// GPS & GNSS Monitor for M5StickS3

// A GPS/GNSS monitoring firmware developed for the M5StickS3.
// It receives and displays real-time data from a connected GPS/GNSS module,
// including position, altitude, speed, course, satellites, HDOP, date, and time.
// The firmware also provides trip and route statistics, configurable units,
// coordinate and time formats, timezone selection, display settings,
// configurable GPS/GNSS baud rate, and persistent settings storage using NVS.

// Developed by ViniciusHNF
// https://github.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor
// https://youtube.com/@ViniciusHNF

// IMPORTANT:
// This firmware requires the M5StickS3 board support package and the required libraries
// to be installed in the Arduino IDE before compiling and uploading.
// For instructions on configuring the M5StickS3 in Arduino IDE, visit:
// https://docs.m5stack.com/en/arduino/m5sticks3/program

// GPS/GNSS TX -> GPIO 44 (GPIO RX)
// GPS/GNSS RX -> GPIO 43 (GPIO TX)

#define GPS_RX_PIN 44  // The TX pin of the GPS/GNSS module must be connected to GPIO 44
#define GPS_TX_PIN 43  // The RX pin of the GPS/GNSS module must be connected to GPIO 43

#include <HardwareSerial.h>  // Library for UART serial communication
#include <M5Unified.h>       // Library for controlling M5Stack hardware
#include <Preferences.h>     // Library for storing persistent settings in NVS
#include <TinyGPSPlus.h>     // Library for parsing GPS/GNSS NMEA data

unsigned long button_b_press_time = 0;  // Variable to store the time when button B is pressed
bool button_b_pressed = false;          // Variable that checks if button B is being pressed
int time_long_press_button_b = 300;     // Variable that stores the duration for which button B must be held down to be considered a long press

byte screen_i = 0;          // Variable to control which screen will be displayed
bool draw_screen = true;    // Variable to control when the screen needs to be redrawn
bool toggle_screen = true;  // Variable to control when the screen has been toggled

bool toggle_screen_settings = true;  // Variable to control when the settings screen has been toggled
bool settings_mode = false;          // Variable to control when the settings mode is active
byte screen_settings_i = 0;          // Variable to control which settings screen will be displayed

int baudrate_options[] = {9600, 19200, 38400, 57600, 115200};                                          // Array of baud rates
uint16_t theme_color_options[] = {WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};                     // Array of available theme colors
String theme_color_options_string[] = {"WHITE", "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA"};  // Array of available theme colors in string format
String units_speed_measurement_options[] = {"km/h", "mph", "knots", "m/s"};                            // Array of available speed measurement units
// Array of available timezones
String timezone_options[] = {"UTC-12:00", "UTC-11:00", "UTC-10:00", "UTC-09:30", "UTC-09:00", "UTC-08:00", "UTC-07:00", "UTC-06:00", "UTC-05:00", "UTC-04:00", "UTC-03:30", "UTC-03:00", "UTC-02:00", "UTC-01:00", "UTC+00:00", "UTC+01:00", "UTC+02:00", "UTC+03:00", "UTC+03:30", "UTC+04:00", "UTC+04:30", "UTC+05:00", "UTC+05:30", "UTC+05:45", "UTC+06:00", "UTC+06:30", "UTC+07:00", "UTC+08:00", "UTC+08:45", "UTC+09:00", "UTC+09:30", "UTC+10:00", "UTC+10:30", "UTC+11:00", "UTC+12:00", "UTC+12:45", "UTC+13:00", "UTC+14:00"};
int screen_timeout_options[] = {0, 10, 30, 60, 300, 600};  // Screen timeout values in seconds

bool new_gps_position = false;        // Indicates that a new valid GPS/GNSS position was received and trip/route data must be updated
bool gps_serial_initialized = false;  // Variable that stores information on whether serial communication has already been initiated. Prevents calling end() on the UART before it has been initialized

struct struct_gps_data {  // Structure for storing data obtained by the GPS/GNSS module
  double latitude;
  double longitude;
  double altitude;
  double speed;
  double course;
  int satellites;
  double hdop;
  String hdop_quality;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

struct struct_gps_TripData {  // Travel data structure
  // Straight-line distance between the starting point and the current position
  double distance_from_start;
  double start_latitude;
  double start_longitude;
  // Route statistics calculated only while the device is moving
  double route_distance;
  double average_moving_speed;
  double route_last_latitude;
  double route_last_longitude;
  unsigned long moving_time_millis;
  unsigned long last_moving_millis;
  // Min/Max
  double max_altitude;
  double min_altitude;
  double max_speed;
  double min_speed;
  // Session
  unsigned long trip_start_millis;
  unsigned long last_millis_shown_info;
};

struct struct_gps_time {  // Date and time structure with timezone and time stamps
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  unsigned long first_fix_millis;    // Stores the millis() value from the moment the first position was obtained
  unsigned long last_update_millis;  // The millis() of the last update
};

struct struct_data_comparison {  // Structure for storing data for comparison
  double latitude;
  double longitude;
  double altitude;
  double speed;
  double course;
  int satellites;
  double hdop;
  String hdop_quality;
  int line_course_x;
  int line_course_y;
  byte battery_percentage;
  bool battery_charging;
};

struct struct_system_state {                        // Structure for storing temporary system states and user interaction timestamps
  bool display_timeout_warning;                     // Stores information on whether the warning is being displayed
  unsigned long last_millis_shown_warning_timeout;  // Store the value of the last time the notice was updated. The purpose is to know when to update again
  unsigned long last_button_interaction_millis;     // Stores the time of the last button interaction
  bool screen_on;                                   // Stores information on whether the screen is on
};

struct struct_settings {             // Structure for storing settings
  int module_baudrate;               // Selected Baud rate
  uint16_t theme_color;              // Variable to control the theme color of the application
  String coord_format;               // Variable for controlling the coordinate format. Decimal || DMS || DM
  String units_speed_measurement;    // Variable to control the speed measurement unit of the application
  String unit_altitude_measurement;  // Variable to control the altitude measurement unit of the application. Meters || Feet
  String unit_distance_measurement;  // Variable for controlling the distance unit of measurement. Metric || Imperial
  String timezone;                   // Variable that stores the selected timezone
  String time_format;                // Variable to control the time format. 24-hour || 12-hour
  String date_format;                // Variable to control the date format. YYYY-MM-DD || MM/DD/YYYY || DD/MM/YYYY
  byte screen_brightness;            // Screen brightness percentage. 100 = 100% brightness
  int screen_timeout_seconds;        // Time in seconds for the screen to turn off when there is no button interaction
  int gps_timeout_seconds;           // Time in seconds to consider that the GPS/GNSS module has not sent new information for a long time
  double min_moving_speed;           // Minimum GPS/GNSS speed in km/h required to record route distance and moving time
};

struct_gps_data gps_data = {};
struct_gps_TripData gps_trip = {};
struct_gps_time gps_time = {};
struct_data_comparison data_comparison = {};
struct_settings settings = {};
struct_system_state system_state = {};

TinyGPSPlus gps;                          // Creates the TinyGPSPlus object for parsing GPS/GNSS data
HardwareSerial gps_serial(1);             // Creates a hardware UART serial interface on UART1
Preferences preferences;                  // Creates the Preferences object for persistent settings storage
#define SETTINGS_NAMESPACE "gpssettings"  // Defines the namespace used to store firmware settings in NVS

void setup() {
  M5.begin();                                                                                   // Initializes the M5 library
  M5.Display.setBrightness(percentage_to_byte(100));                                            // Sets the splash screen brightness to 100%
  M5.Display.fillScreen(BLACK);                                                                 // Fill the screen with the color black
  M5.Display.setRotation(3);                                                                    // Setting the screen rotation to 3
  M5.Display.setTextSize(3);                                                                    // Setting the text size to 3
  M5.Display.setTextColor(WHITE);                                                               // Setting the text color to WHITE
  M5.Display.setTextDatum(middle_center);                                                       // Setting the text datum to middle and center
  M5.Display.drawString("GPS & GNSS", M5.Display.width() / 2, (M5.Display.height() / 2) - 20);  // The text "GPS & GNSS" is displayed in the middle of the screen
  M5.Display.drawString("Monitor", M5.Display.width() / 2, (M5.Display.height() / 2) + 20);     // The text "Monitor" is displayed in the middle of the screen
  M5.Display.setTextDatum(top_left);                                                            // Returning the text datum to the standard
  delay(3000);                                                                                  // Waiting time of 3000 milliseconds

  load_settings();             // Function that retrieves persistent settings and saves them to variables
  if (!validate_settings()) {  // Checks if the settings are valid
    save_settings();           // If any configuration is invalid, save the settings with valid parameters
  }

  M5.Display.setTextColor(settings.theme_color);                             // Setting the text color to settings.theme_color
  M5.Display.setBrightness(percentage_to_byte(settings.screen_brightness));  // Set the screen brightness based on the percentage value
  system_state.screen_on = true;                                             // Sets system_state.screen_on to true, which means the screen is on

  if (gps_serial_initialized) {  // Checks if a serial communication has already been initiated
    gps_serial.end();            // Closes the current UART configuration before applying the new baud rate
  }
  gps_serial.begin(              // Initializes the UART to be used by the GPS/GNSS
      settings.module_baudrate,  // Baud rate
      SERIAL_8N1,                // Configuration: 8 bits, no parity, 1 stop bit
      GPS_RX_PIN,                // RX
      GPS_TX_PIN                 // TX
  );
  gps_serial_initialized = true;  // Sets gps_serial_initialized to true. If another serial communication needs to be initiated, the variable indicates that the previous one must be terminated

  gps_data.hdop_quality = "No Fix";                        // The HDOP quality is set to "No Fix"
  system_state.display_timeout_warning = false;            // system_state.display_timeout_warning is set to false
  system_state.last_button_interaction_millis = millis();  // Stores the value of millis() so that the screen timeout only starts counting at the end of setup()
}

void loop() {
  M5.update();  // Updates the state of the M5StickS3 hardware, especially buttons and input events

  if (M5.BtnB.wasPressed()) {        // Checks if button B was pressed.
    button_b_press_time = millis();  // button_b_press_time receives the value of millis(). millis() returns the time in milliseconds since the ESP32 was powered on.
    button_b_pressed = true;         // button_b_pressed is set to true
  }

  if (settings_mode == false && system_state.display_timeout_warning == false) {  // If configuration mode is disabled and if the timeout warning is not displayed
    // Checks if button B has been released and button_b_pressed is true, or if button B is still pressed, if it has been pressed for longer than time_long_press_button_b and button_b_pressed is true
    if ((M5.BtnB.wasReleased() && button_b_pressed) || (M5.BtnB.isPressed() && millis() - button_b_press_time >= time_long_press_button_b && button_b_pressed)) {
      if (millis() - button_b_press_time < time_long_press_button_b) {  // If the time the button B was pressed is less than time_long_press_button_b
        screen_i = screen_i + 1;                                        // 1 is added to the value of screen_i. Go to the next screen
      } else {                                                          // If the time button B was held down is greater than or equal to time_long_press_button_b
        if (screen_i == 0) {                                            // Checks if screen_i is equal to 0
          screen_i = 10;                                                // screen_i receives the value 10, which is the index of the last screen
        } else {                                                        // If the value of screen_i is not equal to 0
          screen_i = screen_i - 1;                                      // 1 is subtracted from the value of screen_i. Goes to the previous screen
        }
      }
      button_b_pressed = false;  // button_b_pressed is set to false, as the screen has already been toggled
      draw_screen = true;        // Set draw_screen to true to indicate that the screen needs to be redrawn
      toggle_screen = true;      // Set toggle_screen to true to indicate that the screen has been toggled
    }
  }

  if (gps_serial.available() > 0) {  // Checks for new data from the GPS/GNSS module
    update_gps_data();               // Updates GPS data
    if (new_gps_position) {          // Checks for a new position
      update_gps_trip();             // Updates trip data such as distance from the starting point and minimum/maximum values
      update_gps_route();            // Updates route statistics such as traveled distance and average moving speed
      new_gps_position = false;
    }
    draw_screen = true;  // indicates that the screen needs to be redrawn because there are GPS updates
  }

  // Checks if the screen timeout is enabled and if the time elapsed since the last button press exceeds the screen timeout
  if (settings.screen_timeout_seconds > 0 && millis() - system_state.last_button_interaction_millis > (settings.screen_timeout_seconds * 1000)) {
    if (system_state.screen_on) {      // If the screen is on
      M5.Display.setBrightness(0);     // Sets the screen brightness to 0. this turns off the screen
      system_state.screen_on = false;  // Indicates that the screen is off
    }
  } else {
    if (system_state.screen_on == false) {                                       // If the screen is off
      M5.Display.setBrightness(percentage_to_byte(settings.screen_brightness));  // Set the screen brightness based on the percentage value
      system_state.screen_on = true;                                             // Indicates that the screen is now on
      draw_screen = true;                                                        // Set draw_screen to true to indicate that the screen needs to be redrawn
      toggle_screen = true;                                                      // Set toggle_screen to true to indicate that the screen has been toggled
    }
  }

  // Checks if the time interval is greater than the time limit
  if (gps_time.last_update_millis != 0 && millis() - gps_time.last_update_millis >= (settings.gps_timeout_seconds * 1000)) {
    // Checks if the time interval is one second greater than the time limit
    if (millis() - gps_time.last_update_millis < ((settings.gps_timeout_seconds + 1) * 1000)) {
      system_state.display_timeout_warning = true;  // Enable Communication Warning
    }
  } else {
    // If it was showing the warning and communication was restored
    if (system_state.display_timeout_warning) {
      toggle_screen = true;  // Enables indicating that the screen needs to be drawn
      draw_screen = true;    // Enables indicating that the screen needs to be drawn
    }
    system_state.display_timeout_warning = false;  // Remove Communication Warning
  }

  if (system_state.display_timeout_warning) {  // Whether the warning should be displayed on the screen
    // The screen must be redrawn every second to show the duration of the communication issues
    if (millis() - system_state.last_millis_shown_warning_timeout > 1000) {
      system_state.last_millis_shown_warning_timeout = millis();
      draw_screen = true;
    }
    if (draw_screen) {
      screen_header("Warning");
      M5.Display.setTextSize(2);
      M5.Display.setTextDatum(top_center);
      M5.Display.drawString("Communication", M5.Display.width() / 2, 35);
      M5.Display.drawString("Warning", M5.Display.width() / 2, 55);
      M5.Display.setTextSize(1);
      M5.Display.drawString("No updates received for " + millis_to_time(millis() - gps_time.last_update_millis), M5.Display.width() / 2, 90);  // Shows the elapsed time
      M5.Display.drawString("Press Button A to dismiss", M5.Display.width() / 2, 115);
      draw_screen = false;  // draw_screen is set to false so the screen isn't drawn every time
    }
    if (M5.BtnA.wasPressed()) {                      // If button A was pressed
      system_state.display_timeout_warning = false;  // Dismiss the warning
      toggle_screen = true;
      draw_screen = true;
    }
  } else {
    if (system_state.screen_on) {  // Checks if the screen is on
      update_display();            // Function responsible for selecting which screen will be displayed
    } else {
      delay(25);  // Reduces loop frequency while the screen is off to help reduce power consumption
    }
  }

  // Checks if any button has been pressed to determine when the screen should turn off, if the screen timeout is enabled
  if (M5.BtnA.wasPressed() || M5.BtnA.wasReleased() || M5.BtnB.wasPressed() || M5.BtnB.wasReleased()) {
    system_state.last_button_interaction_millis = millis();  // Stores the millis() value from when a button was pressed
  }
}

void update_gps_data() {                // Function responsible for verifying and updating GPS/GNSS module data
  while (gps_serial.available() > 0) {  // Checks for new valid information from the GPS/GNSS module
    gps.encode(gps_serial.read());      // reads data from the GPS/GNSS module
  }
  if (gps.location.isUpdated() && gps.location.isValid()) {  // Position
    gps_data.latitude = gps.location.lat();
    gps_data.longitude = gps.location.lng();
    gps_time.last_update_millis = millis();  // Stores the millis() value where the last valid information was received
    new_gps_position = true;
    if (gps_time.first_fix_millis == 0) {
      gps_time.first_fix_millis = millis();  // Stores the time of the first valid position fix since startup
    }
  }
  if (gps.altitude.isUpdated() && gps.altitude.isValid()) {  // Altitude
    gps_data.altitude = gps.altitude.meters();
    gps_time.last_update_millis = millis();  // Stores the millis() value where the last valid information was received
  }
  if (gps.speed.isUpdated() && gps.speed.isValid()) {  // Speed
    gps_data.speed = gps.speed.kmph();
  }
  if (gps.course.isUpdated() && gps.course.isValid()) {  // Course
    gps_data.course = gps.course.deg();
  }
  if (gps.satellites.isUpdated() && gps.satellites.isValid()) {  // Satellites
    gps_data.satellites = gps.satellites.value();
    gps_time.last_update_millis = millis();  // Stores the millis() value where the last valid information was received
  }
  if (gps.hdop.isUpdated() && gps.hdop.isValid()) {  // HDOP
    gps_data.hdop = gps.hdop.hdop();
  }
  if (gps.date.isUpdated() && gps.date.isValid()) {  // Date
    gps_data.year = gps.date.year();
    gps_data.month = gps.date.month();
    gps_data.day = gps.date.day();
    update_date_time();
  }
  if (gps.time.isUpdated() && gps.time.isValid()) {  // Time
    gps_data.hour = gps.time.hour();
    gps_data.minute = gps.time.minute();
    gps_data.second = gps.time.second();
    update_date_time();
    gps_time.last_update_millis = millis();  // Stores the millis() value where the last valid information was received
  }
  // Defines the HDOP quality based on the HDOP value
  if (gps_data.hdop < 0.7) {
    if (gps_data.hdop == 0) {
      gps_data.hdop_quality = "No Fix";  // When the HDOP is 0, it means the variable holds its default value, indicating that no fix has been obtained since initialization
    } else {
      gps_data.hdop_quality = "Excellent";
    }
  } else if (gps_data.hdop < 1.5) {
    gps_data.hdop_quality = "Good";
  } else if (gps_data.hdop < 3.0) {
    gps_data.hdop_quality = "Moderate";
  } else if (gps_data.hdop < 5.0) {
    gps_data.hdop_quality = "Poor";
  } else {
    gps_data.hdop_quality = "Very Poor";
  }
}

void update_gps_trip() {  // Updates trip statistics using the current valid GPS/GNSS position
  if (gps_data.latitude != 0.0 && gps_data.longitude != 0.0) {
    if (gps_trip.trip_start_millis == 0) {
      gps_trip.trip_start_millis = millis();  // Stores the trip start time
    }
    if (gps_trip.start_latitude == 0.0 && gps_trip.start_longitude == 0.0) {
      gps_trip.start_latitude = gps_data.latitude;
      gps_trip.start_longitude = gps_data.longitude;
    } else {
      gps_trip.distance_from_start = TinyGPSPlus::distanceBetween(gps_trip.start_latitude, gps_trip.start_longitude, gps_data.latitude, gps_data.longitude);  // Measures the distance between two points
    }
  }
  if (gps_data.altitude > gps_trip.max_altitude) {
    gps_trip.max_altitude = gps_data.altitude;
  }
  if (gps_data.altitude < gps_trip.min_altitude || gps_trip.min_altitude == 0.0) {
    gps_trip.min_altitude = gps_data.altitude;
  }
  if (gps_data.speed > gps_trip.max_speed) {
    gps_trip.max_speed = gps_data.speed;
  }
  if (gps_data.speed < gps_trip.min_speed || gps_trip.min_speed == 0.0) {
    gps_trip.min_speed = gps_data.speed;
  }
}

void update_gps_route() {  // Accumulates route distance and moving time using consecutive GPS/GNSS positions
  if (gps_data.latitude == 0.0 && gps_data.longitude == 0.0) {
    return;
  }
  if (gps_data.speed <= settings.min_moving_speed) {  // Ignore position changes while stopped to prevent GPS position drift from increasing the route distance
    gps_trip.route_last_latitude = 0.0;
    gps_trip.route_last_longitude = 0.0;
    gps_trip.last_moving_millis = 0;
    return;
  }
  unsigned long current_millis = millis();
  if (gps_trip.route_last_latitude == 0.0 &&
      gps_trip.route_last_longitude == 0.0) {
    gps_trip.route_last_latitude = gps_data.latitude;
    gps_trip.route_last_longitude = gps_data.longitude;
    gps_trip.last_moving_millis = current_millis;
    return;
  }
  double segment_distance = TinyGPSPlus::distanceBetween(gps_trip.route_last_latitude, gps_trip.route_last_longitude, gps_data.latitude, gps_data.longitude);
  gps_trip.route_distance += segment_distance;  // Adds the distance between the previous and current moving positions to the total route distance
  if (gps_trip.last_moving_millis != 0) {
    gps_trip.moving_time_millis += current_millis - gps_trip.last_moving_millis;
  }
  gps_trip.last_moving_millis = current_millis;
  gps_trip.route_last_latitude = gps_data.latitude;
  gps_trip.route_last_longitude = gps_data.longitude;
  if (gps_trip.moving_time_millis > 0) {
    gps_trip.average_moving_speed = (gps_trip.route_distance / 1000.0) / (gps_trip.moving_time_millis / 3600000.0);  // Calculates average moving speed in km/h using route distance and moving time
  } else {
    gps_trip.average_moving_speed = 0;
  }
}

void update_display() {  // Function responsible for selecting which screen will be displayed
  switch (screen_i) {    // Check the screen_i index
    case 0:              // If the screen_i index is 0, it goes to the general screen
      screen_general();
      break;
    case 1:  // If the screen_i index is 1, it goes to the position screen
      screen_position();
      break;
    case 2:  // If the screen_i index is 2, it goes to the speed screen.
      screen_speed();
      break;
    case 3:  // If the screen_i index is 2, it goes to the altitude screen
      screen_altitude();
      break;
    case 4:
      screen_distance();
      break;
    case 5:
      screen_route_stats();
      break;
    case 6:
      screen_course();
      break;
    case 7:
      screen_time();
      break;
    case 8:
      screen_session_info();
      break;
    case 9:
      screen_about();
      break;
    case 10:
      screen_settings();
      break;
    default:  // If the screen_i index is not between 0 and 10, it goes to the general screen and screen_i resets to 0
      screen_general();
      screen_i = 0;
      break;
  }
  draw_screen = false;    // draw_screen is set to false, as the screen has already been drawn
  toggle_screen = false;  // toggle_screen is set to false because the screen has already been toggled
}

void screen_general() {
  if (draw_screen) {
    screen_header("General");
    M5.Display.setCursor(10, 35);
    M5.Display.setTextSize(1);
    M5.Display.print("SAT: ");
    M5.Display.setTextSize(2);
    M5.Display.println(gps_data.satellites);
    M5.Display.setCursor(130, 35);
    M5.Display.setTextSize(1);
    M5.Display.print("HDOP: ");
    M5.Display.setTextSize(2);
    M5.Display.println(gps_data.hdop, 1);
    M5.Display.setCursor(10, 65);
    M5.Display.setTextSize(1);
    M5.Display.print("SPEED: ");
    M5.Display.setTextSize(2);
    M5.Display.println(convert_speed(gps_data.speed, false));
    M5.Display.setCursor(130, 65);
    M5.Display.setTextSize(1);
    M5.Display.print("ALT: ");
    M5.Display.setTextSize(2);
    if (settings.unit_altitude_measurement == "Meters") {
      M5.Display.println(gps_data.altitude, 0);
    } else {
      M5.Display.println(gps_data.altitude * 3.28084, 0);
    }
    M5.Display.setCursor(10, 95);
    M5.Display.setTextSize(1);
    M5.Display.print("LAT: ");
    M5.Display.setTextSize(2);
    M5.Display.println(gps_data.latitude, 6);
    M5.Display.setCursor(10, 115);
    M5.Display.setTextSize(1);
    M5.Display.print("LNG: ");
    M5.Display.setTextSize(2);
    M5.Display.println(gps_data.longitude, 6);
  }
}

void screen_position() {
  toggle_coord_format();
  if (draw_screen) {
    String latitude_str;
    String longitude_str;
    bool draw_degree_symbol = false;
    int latitude_degree_x = 0;
    int longitude_degree_x = 0;
    if (settings.coord_format == "Decimal") {
      latitude_str = String(gps_data.latitude, 6);
      longitude_str = String(gps_data.longitude, 6);
    } else if (settings.coord_format == "DM") {
      {  // Latitude
        double coordinate = gps_data.latitude;
        char direction = coordinate >= 0 ? 'N' : 'S';
        coordinate = abs(coordinate);
        int degrees = (int)coordinate;
        double minutes = (coordinate - degrees) * 60.0;
        char buffer[20];
        sprintf(buffer, "%d %.4f'%c", degrees, minutes, direction);
        latitude_str = String(buffer);
      }
      {  // Longitude
        double coordinate = gps_data.longitude;
        char direction = coordinate >= 0 ? 'E' : 'W';
        coordinate = abs(coordinate);
        int degrees = (int)coordinate;
        double minutes = (coordinate - degrees) * 60.0;
        char buffer[20];
        sprintf(buffer, "%d %.4f'%c", degrees, minutes, direction);
        longitude_str = String(buffer);
      }
      draw_degree_symbol = true;
    } else if (settings.coord_format == "DMS") {
      {  // Latitude
        double coordinate = gps_data.latitude;
        char direction = coordinate >= 0 ? 'N' : 'S';
        coordinate = abs(coordinate);
        int degrees = (int)coordinate;
        double minutes_decimal = (coordinate - degrees) * 60.0;
        int minutes = (int)minutes_decimal;
        double seconds = (minutes_decimal - minutes) * 60.0;
        char buffer[25];
        sprintf(buffer, "%d %02d'%.1f\"%c", degrees, minutes, seconds, direction);
        latitude_str = String(buffer);
      }
      {  // Longitude
        double coordinate = gps_data.longitude;
        char direction = coordinate >= 0 ? 'E' : 'W';
        coordinate = abs(coordinate);
        int degrees = (int)coordinate;
        double minutes_decimal = (coordinate - degrees) * 60.0;
        int minutes = (int)minutes_decimal;
        double seconds = (minutes_decimal - minutes) * 60.0;
        char buffer[25];
        sprintf(buffer, "%d %02d'%.1f\"%c", degrees, minutes, seconds, direction);
        longitude_str = String(buffer);
      }
      draw_degree_symbol = true;
    }
    screen_header("Position");
    M5.Display.setTextSize(2);
    int latitude_y = 35;
    int longitude_y = 60;
    M5.Display.setCursor(10, latitude_y);
    M5.Display.print("LAT: ");
    int latitude_start_x = M5.Display.getCursorX();
    M5.Display.println(latitude_str);
    M5.Display.setCursor(10, longitude_y);
    M5.Display.print("LNG: ");
    int longitude_start_x = M5.Display.getCursorX();
    M5.Display.println(longitude_str);
    if (draw_degree_symbol) {
      // Latitude
      int latitude_degrees = abs((int)gps_data.latitude);
      int latitude_degree_width = M5.Display.textWidth(String(latitude_degrees));
      latitude_degree_x = latitude_start_x + latitude_degree_width + 2;
      M5.Display.drawCircle(latitude_degree_x, latitude_y + 3, 2);
      // Longitude
      int longitude_degrees = abs((int)gps_data.longitude);
      int longitude_degree_width = M5.Display.textWidth(String(longitude_degrees));
      longitude_degree_x = longitude_start_x + longitude_degree_width + 2;
      M5.Display.drawCircle(longitude_degree_x, longitude_y + 3, 2);
    }
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 100);
    M5.Display.print("HDOP: ");
    M5.Display.print(gps_data.hdop, 1);
    M5.Display.setCursor(120, 100);
    M5.Display.print("Satellites: ");
    M5.Display.print(gps_data.satellites);
    M5.Display.setCursor(10, 115);
    M5.Display.print("Position Quality: ");
    M5.Display.print(gps_data.hdop_quality);
  }
}

void screen_speed() {
  toggle_speed_unit();
  if (toggle_screen || (int)data_comparison.speed != (int)gps_data.speed || data_comparison.hdop_quality != gps_data.hdop_quality) {
    screen_header("Speed");
    M5.Display.setTextSize(4);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString(convert_speed(gps_data.speed, false), M5.Display.width() / 2, 57);
    int speed_text_width = M5.Display.width() / 2 + M5.Display.textWidth(convert_speed(gps_data.speed, false)) / 2;
    M5.Display.setTextDatum(top_left);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(speed_text_width + 3, 54);
    M5.Display.print(settings.units_speed_measurement);
    M5.Display.setCursor(10, 90);
    M5.Display.print("Min: ");
    M5.Display.print(convert_speed(gps_trip.min_speed, false));
    M5.Display.setCursor(120, 90);
    M5.Display.print("Max: ");
    M5.Display.print(convert_speed(gps_trip.max_speed, false));
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 115);
    M5.Display.print("Position Quality: ");
    M5.Display.print(gps_data.hdop_quality);
    equalize_comparison();
  }
}

void screen_altitude() {
  toggle_altitude_unit();
  if (toggle_screen || (int)data_comparison.altitude != (int)gps_data.altitude || data_comparison.hdop_quality != gps_data.hdop_quality) {
    screen_header("Altitude");
    M5.Display.setTextSize(4);
    M5.Display.setTextDatum(middle_center);
    if (settings.unit_altitude_measurement == "Meters") {
      M5.Display.drawString(String((int)gps_data.altitude), M5.Display.width() / 2, 57);
      int altitude_text_width = M5.Display.width() / 2 + M5.Display.textWidth(String((int)gps_data.altitude)) / 2;
      M5.Display.setTextDatum(top_left);
      M5.Display.setTextSize(2);
      M5.Display.setCursor(altitude_text_width + 3, 54);
      M5.Display.print("m");
      M5.Display.setCursor(10, 90);
      M5.Display.print("Min: ");
      M5.Display.print((int)gps_trip.min_altitude);
      M5.Display.setCursor(120, 90);
      M5.Display.print("Max: ");
      M5.Display.print((int)gps_trip.max_altitude);
    } else {
      M5.Display.drawString(String((int)(gps_data.altitude * 3.28084)), (M5.Display.width() / 2) - 10, 57);
      int altitude_text_width = (M5.Display.width() / 2) - 10 + M5.Display.textWidth(String((int)(gps_data.altitude * 3.28084))) / 2;
      M5.Display.setTextDatum(top_left);
      M5.Display.setTextSize(2);
      M5.Display.setCursor(altitude_text_width + 3, 54);
      M5.Display.print("ft");
      M5.Display.setCursor(10, 90);
      M5.Display.setTextSize(1);
      M5.Display.print("Min: ");
      M5.Display.setTextSize(2);
      M5.Display.print((int)(gps_trip.min_altitude * 3.28084));
      M5.Display.setCursor(120, 90);
      M5.Display.setTextSize(1);
      M5.Display.print("Max: ");
      M5.Display.setTextSize(2);
      M5.Display.print((int)(gps_trip.max_altitude * 3.28084));
    }
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 115);
    M5.Display.print("Position Quality: ");
    M5.Display.print(gps_data.hdop_quality);
    equalize_comparison();
  }
}

void screen_distance() {  // Displays the straight-line distance from the trip starting point
  toggle_distance_unit();
  if (draw_screen) {
    String distance_str;
    if (settings.unit_distance_measurement == "Metric") {
      if (gps_trip.distance_from_start < 1000.0) {
        distance_str = String((int)gps_trip.distance_from_start) + " m";
      } else {
        distance_str = String(gps_trip.distance_from_start / 1000.0, 1) + " km";
      }
    } else {
      double feet = gps_trip.distance_from_start * 3.28084;
      if (feet < 5280.0) {
        distance_str = String((int)feet) + " ft";
      } else {
        double miles = gps_trip.distance_from_start * 0.000621371;
        distance_str = String(miles, 1) + " mi";
      }
    }
    screen_header("Distance");
    M5.Display.setTextSize(4);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString(String(distance_str), M5.Display.width() / 2, 55);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Straight-line distance", M5.Display.width() / 2, 85);
    M5.Display.setTextDatum(top_left);
    M5.Display.setCursor(30, 100);
    M5.Display.print("Current");
    M5.Display.setCursor(150, 100);
    M5.Display.print("Reference");
    M5.Display.setCursor(10, 110);
    M5.Display.print("Lat: ");
    M5.Display.print(gps_data.latitude, 6);
    M5.Display.setCursor(10, 120);
    M5.Display.print("Lng: ");
    M5.Display.print(gps_data.longitude, 6);
    M5.Display.setCursor(130, 110);
    M5.Display.print("Lat: ");
    M5.Display.print(gps_trip.start_latitude, 6);
    M5.Display.setCursor(130, 120);
    M5.Display.print("Lng: ");
    M5.Display.print(gps_trip.start_longitude, 6);
    M5.Display.drawLine(115, 95, 115, 135, settings.theme_color);
    M5.Display.drawLine(0, 95, 240, 95, settings.theme_color);
  }
}

void screen_route_stats() {  // Displays the accumulated route distance and average moving speed
  if (draw_screen || toggle_screen) {
    screen_header("Route Stats");
    String distance_str;
    if (settings.unit_distance_measurement == "Metric") {
      if (gps_trip.route_distance < 1000.0) {
        distance_str = String((int)gps_trip.route_distance) + " m";
      } else {
        distance_str = String(gps_trip.route_distance / 1000.0, 2) + " km";
      }
    } else {
      double feet = gps_trip.route_distance * 3.28084;
      if (feet < 5280.0) {
        distance_str = String((int)feet) + " ft";
      } else {
        double miles = gps_trip.route_distance * 0.000621371;
        distance_str = String(miles, 1) + " mi";
      }
    }
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(top_center);
    M5.Display.drawString("Distance", M5.Display.width() / 2, 35);
    M5.Display.setTextSize(3);
    M5.Display.drawString(distance_str, M5.Display.width() / 2, 50);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Average Moving Speed", M5.Display.width() / 2, 85);
    M5.Display.setTextSize(3);
    if (gps_trip.average_moving_speed == 0) {
      M5.Display.drawString("No Data", M5.Display.width() / 2, 100);
    } else {
      M5.Display.drawString(convert_speed(gps_trip.average_moving_speed, true), M5.Display.width() / 2, 100);
    }
    M5.Display.setTextDatum(top_left);
  }
}

void screen_course() {
  if (toggle_screen) {
    M5.Display.fillScreen(BLACK);
    M5.Display.fillRect(0, 0, 173, 25, settings.theme_color);
    M5.Display.fillRect(0, 110, 173, 25, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(BLACK);
    M5.Display.setTextDatum(top_left);
    M5.Display.drawString("Course", 10, 5);
    M5.Display.fillCircle(172, 67, 73, BLACK);
    M5.Display.fillCircle(172, 67, 67, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(BLACK);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("N", 172, 14);
    M5.Display.drawString("NE", 208, 33);
    M5.Display.drawString("E", 230, 67);
    M5.Display.drawString("SE", 208, 103);
    M5.Display.drawString("S", 172, 124);
    M5.Display.drawString("SW", 137, 103);
    M5.Display.drawString("W", 116, 67);
    M5.Display.drawString("NW", 137, 33);
  }

  if (draw_screen || toggle_screen) {
    int centerX = 172;
    int centerY = 67;
    int pointerLength = 35;
    float angle = gps_data.course * PI / 180.0;
    int endX = centerX + sin(angle) * pointerLength;
    int endY = centerY - cos(angle) * pointerLength;
    if (data_comparison.line_course_x != endX || data_comparison.line_course_y != endY || toggle_screen) {
      if (data_comparison.line_course_x != 0 && data_comparison.line_course_y != 0) {
        M5.Display.drawWideLine(centerX, centerY, data_comparison.line_course_x, data_comparison.line_course_y, 5, settings.theme_color);
      }
      M5.Display.drawWideLine(centerX, centerY, endX, endY, 3, BLACK);  // Draws a wide black line from the center to the pointer endpoint
      data_comparison.line_course_x = endX;
      data_comparison.line_course_y = endY;

      String direction;
      String direction_symbol;
      if (gps_data.course >= 337.5 || gps_data.course < 22.5) {
        direction = "North";
        direction_symbol = "N";
      } else if (gps_data.course < 67.5) {
        direction = "Northeast";
        direction_symbol = "NE";
      } else if (gps_data.course < 112.5) {
        direction = "East";
        direction_symbol = "E";
      } else if (gps_data.course < 157.5) {
        direction = "Southeast";
        direction_symbol = "SE";
      } else if (gps_data.course < 202.5) {
        direction = "South";
        direction_symbol = "S";
      } else if (gps_data.course < 247.5) {
        direction = "Southwest";
        direction_symbol = "SW";
      } else if (gps_data.course < 292.5) {
        direction = "West";
        direction_symbol = "W";
      } else {
        direction = "Northwest";
        direction_symbol = "NW";
      }
      M5.Display.fillCircle(172, 67, 7, BLACK);
      M5.Display.fillCircle(172, 67, 2, settings.theme_color);
      M5.Display.fillRect(0, 25, 103, 85, BLACK);
      M5.Display.setTextDatum(top_left);
      M5.Display.setTextSize(3);
      M5.Display.setTextColor(settings.theme_color);
      M5.Display.setCursor(10, 40);
      M5.Display.print((int)gps_data.course);
      int course_text_width = M5.Display.textWidth(String((int)gps_data.course));
      M5.Display.drawCircle(10 + course_text_width + 5, 45, 3, settings.theme_color);
      M5.Display.drawCircle(10 + course_text_width + 5, 45, 4, settings.theme_color);
      M5.Display.setCursor(10, 75);
      M5.Display.setTextSize(1);
      M5.Display.print(String(direction));
      M5.Display.setCursor(10, 90);
      M5.Display.print(convert_speed(gps_data.speed, true));
      M5.Display.fillRect(0, 110, 103, 25, settings.theme_color);
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(BLACK);
      M5.Display.setTextDatum(bottom_left);
      if (M5.Power.isCharging()) {
        M5.Display.drawString("+" + String(M5.Power.getBatteryLevel()) + "%", 10, M5.Display.height() - 3);
      } else {
        M5.Display.drawString(String(M5.Power.getBatteryLevel()) + "%", 10, M5.Display.height() - 3);
      }
      M5.Display.setTextColor(settings.theme_color);
      M5.Display.setTextDatum(top_left);
    }
  }
}

void screen_time() {
  if (M5.BtnA.wasPressed()) {
    toggle_time_format();
  }
  if (draw_screen) {
    screen_header("Time");
    char time_str[12];
    if (settings.time_format == "24-hour") {
      sprintf(time_str, "%02d:%02d:%02d", gps_time.hour, gps_time.minute, gps_time.second);  // Formats the time as HH:MM:SS
    } else {
      int hour = gps_time.hour;
      const char* period;
      if (hour >= 12) {
        period = "PM";
      } else {
        period = "AM";
      }
      hour = hour % 12;
      if (hour == 0) {
        hour = 12;
      }
      sprintf(time_str, "%02d:%02d %s", hour, gps_time.minute, period);  // Formats the time using the 12-hour format with AM or PM
    }
    char date_str[11];
    if (settings.date_format == "YYYY-MM-DD") {
      sprintf(date_str, "%04d-%02d-%02d", gps_time.year, gps_time.month, gps_time.day);
    } else if (settings.date_format == "MM/DD/YYYY") {
      sprintf(date_str, "%02d/%02d/%04d", gps_time.month, gps_time.day, gps_time.year);
    } else {
      sprintf(date_str, "%02d/%02d/%04d", gps_time.day, gps_time.month, gps_time.year);
    }
    M5.Display.setTextSize(4);
    M5.Display.setTextDatum(top_center);
    M5.Display.drawString(time_str, M5.Display.width() / 2, 35);
    M5.Display.setTextSize(3);
    M5.Display.drawString(date_str, M5.Display.width() / 2, 80);
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(bottom_center);
    M5.Display.drawString("Timezone: " + settings.timezone, M5.Display.width() / 2, M5.Display.height() - 10);
    M5.Display.setTextDatum(top_left);
  }
}

void screen_session_info() {
  if (millis() - gps_trip.last_millis_shown_info > 1000 && millis() - gps_time.last_update_millis >= (settings.gps_timeout_seconds * 1000)) {
    gps_trip.last_millis_shown_info = millis();
    draw_screen = true;
  }
  if (draw_screen || toggle_screen) {
    screen_header("Session Info");
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 35);
    M5.Display.print("Trip Time");
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 50);
    if (gps_trip.trip_start_millis == 0) {
      M5.Display.print("No Fix");
    } else {
      M5.Display.print(get_elapsed_time(gps_trip.trip_start_millis));
    }
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 75);
    M5.Display.print("First Fix");
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 90);
    if (gps_time.first_fix_millis == 0) {
      M5.Display.print("No Fix");
    } else {
      M5.Display.print(get_elapsed_time(gps_time.first_fix_millis));
      M5.Display.print(" ago");
    }
    M5.Display.setCursor(10, 120);
    M5.Display.setTextSize(1);
    M5.Display.print("Fix Time: ");
    M5.Display.print(millis_to_time(gps_time.first_fix_millis));
  }
}

void screen_about() {
  if (toggle_screen) {
    screen_header("About");
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 30);
    M5.Display.print("Developed by ViniciusHNF");
    M5.Display.setCursor(10, 45);
    M5.Display.print("> github.com/viniciushnf");
    M5.Display.setCursor(10, 60);
    M5.Display.print("> youtube.com/@ViniciusHNF");
    M5.Display.setCursor(10, 90);
    M5.Display.print("Selected baud rate: ");
    M5.Display.print(settings.module_baudrate);
    M5.Display.setCursor(10, 105);
    M5.Display.print("GPS/GNSS PIN TX -> GPIO ");
    M5.Display.print(GPS_RX_PIN);
    M5.Display.print(" (GPIO RX)");
    M5.Display.setCursor(10, 120);
    M5.Display.print("GPS/GNSS PIN RX -> GPIO ");
    M5.Display.print(GPS_TX_PIN);
    M5.Display.print(" (GPIO TX)");
  }
}

void screen_settings() {
  if (settings_mode) {    // If in configuration mode
    draw_screen = false;  // draw_screen is set to false to avoid unnecessary drawing on the settings screen caused by GPS updates.
    // Checks if button B has been released and button_b_pressed is true, or if button B is still pressed, if it has been pressed for longer than time_long_press_button_b and button_b_pressed is true
    if ((M5.BtnB.wasReleased() && button_b_pressed) || (M5.BtnB.isPressed() && millis() - button_b_press_time >= time_long_press_button_b && button_b_pressed)) {
      if (millis() - button_b_press_time < time_long_press_button_b) {  // If the time the button B was pressed is less than time_long_press_button_b
        screen_settings_i = screen_settings_i + 1;                      // 1 is added to the value of screen_settings_i
      } else {                                                          // If the time button B was held down is greater than or equal to time_long_press_button_b
        if (screen_settings_i == 0) {                                   // Checks if screen_settings_i is equal to 0
          screen_settings_i = 12;                                       // screen_settings_i receives the value 12, which is the index of the last screen
        } else {                                                        // If the value of screen_settings_i is not equal to 0
          screen_settings_i = screen_settings_i - 1;                    // 1 is subtracted from the value of screen_settings_i. Goes to the previous screen
        }
      }
      button_b_pressed = false;       // button_b_pressed is set to false, as the screen has already been toggled
      draw_screen = true;             // Set draw_screen to true to indicate that the screen needs to be redrawn
      toggle_screen = true;           // Set toggle_screen to true to indicate that the screen has been toggled
      toggle_screen_settings = true;  // toggle_screen_settings is set to true, indicating that the configuration screen needs to be drawn.
    }
    switch (screen_settings_i) {
      case 0:
        screen_settings_color();
        break;
      case 1:
        screen_settings_brightness();
        break;
      case 2:
        screen_settings_screen_timeout();
        break;
      case 3:
        screen_settings_coord_format();
        break;
      case 4:
        screen_settings_unit_speed();
        break;
      case 5:
        screen_settings_unit_altitude();
        break;
      case 6:
        screen_settings_unit_distance();
        break;
      case 7:
        screen_settings_timezone();
        break;
      case 8:
        screen_settings_time_format();
        break;
      case 9:
        screen_settings_date_format();
        break;
      case 10:
        screen_settings_baudrate();
        break;
      case 11:
        screen_settings_reset_trip();
        break;
      case 12:
        screen_settings_exit();
        break;
      default:
        screen_settings_color();
        screen_settings_i = 0;
        break;
    }
    toggle_screen_settings = false;
  } else {
    if (toggle_screen) {
      screen_header("Settings");
      M5.Display.setTextSize(2);
      M5.Display.setTextDatum(middle_center);
      M5.Display.drawString("Press the A", M5.Display.width() / 2, 55);
      M5.Display.drawString("button to enter", M5.Display.width() / 2, 75);
      M5.Display.drawString("the settings", M5.Display.width() / 2, 95);
    }
    if (M5.BtnA.wasPressed()) {  // If button A is pressed
      settings_mode = true;      // Enables configuration mode
      toggle_screen_settings = true;
      screen_settings_i = 0;
    }
  }
}

void screen_settings_color() {
  byte selected_color_i = 0;  // Finding the already defined theme color
  for (int i_color = 0; i_color < sizeof(theme_color_options) / sizeof(theme_color_options[0]); i_color++) {
    if (theme_color_options[i_color] == settings.theme_color) {
      selected_color_i = i_color;
    }
  }
  if (M5.BtnA.wasPressed()) {
    selected_color_i = selected_color_i + 1;                                                      // 1 is added to the value of selected_color_i
    if (selected_color_i > (sizeof(theme_color_options) / sizeof(theme_color_options[0])) - 1) {  // If the value of selected_color_i is equal to the number of indices in theme_color_options
      selected_color_i = 0;                                                                       // selected_color_i returns to 0
    }
    settings.theme_color = theme_color_options[selected_color_i];  // The theme_color will be the selected_color_i index of the theme_color_options array
    draw_screen = true;
    toggle_screen_settings = true;  // toggle_screen_settings is set to true to force the header to update with the new color
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Color");
    show_defined_configuration(theme_color_options_string[selected_color_i]);
    settings_footer("color");
  }
}

void screen_settings_brightness() {
  if (M5.BtnA.wasPressed()) {
    settings.screen_brightness = settings.screen_brightness + 10;  // 10 is added to the value of settings.screen_brightness
    if (settings.screen_brightness > 100) {                        // If the value of settings.screen_brightness is greater than 100
      settings.screen_brightness = 10;                             // settings.screen_brightness returns to 10
    }
    M5.Display.setBrightness(percentage_to_byte(settings.screen_brightness));  // Set the screen brightness based on the percentage value
    draw_screen = true;
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Brightness");
    show_defined_configuration(String(settings.screen_brightness) + "%");
    settings_footer("brightness");
  }
}

void screen_settings_screen_timeout() {
  byte selected_timeout_i = 0;
  // Find the current timeout value in the available options
  for (int i_timeout = 0; i_timeout < sizeof(screen_timeout_options) / sizeof(screen_timeout_options[0]); i_timeout++) {
    if (screen_timeout_options[i_timeout] == settings.screen_timeout_seconds) {
      selected_timeout_i = i_timeout;
      break;
    }
  }
  if (M5.BtnA.wasPressed()) {
    selected_timeout_i = selected_timeout_i + 1;
    if (selected_timeout_i >= sizeof(screen_timeout_options) / sizeof(screen_timeout_options[0])) {
      selected_timeout_i = 0;
    }
    settings.screen_timeout_seconds = screen_timeout_options[selected_timeout_i];  // Apply the selected timeout value, stored internally in seconds
    draw_screen = true;
    save_settings();
  }
  String str_screen_timeout = "";
  if (settings.screen_timeout_seconds == 0) {
    str_screen_timeout = "Disabled";
  } else if (settings.screen_timeout_seconds >= 60) {
    str_screen_timeout = String(settings.screen_timeout_seconds / 60) + " minute" + (settings.screen_timeout_seconds >= 120 ? "s" : "");
  } else {
    str_screen_timeout = String(settings.screen_timeout_seconds) + " seconds";
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Screen Timeout");
    show_defined_configuration(str_screen_timeout);
    settings_footer("screen timeout");
  }
}

void screen_settings_coord_format() {
  toggle_coord_format();
  if (draw_screen || toggle_screen_settings) {
    screen_header("Coord. Format");
    show_defined_configuration(settings.coord_format);
    settings_footer("coord. format");
  }
}

void screen_settings_unit_speed() {
  toggle_speed_unit();
  if (draw_screen || toggle_screen_settings) {
    screen_header("Speed Unit");
    show_defined_configuration(settings.units_speed_measurement);
    settings_footer("speed unit");
  }
}

void screen_settings_unit_altitude() {
  toggle_altitude_unit();
  if (draw_screen || toggle_screen_settings) {
    screen_header("Altitude Unit");
    show_defined_configuration(settings.unit_altitude_measurement);
    settings_footer("altitude unit");
  }
}

void screen_settings_unit_distance() {
  toggle_distance_unit();
  if (draw_screen || toggle_screen_settings) {
    screen_header("Distance Unit");
    show_defined_configuration(settings.unit_distance_measurement);
    settings_footer("distance unit");
  }
}

void screen_settings_timezone() {
  byte selected_timezone_i = 0;  // Finding the already defined timezone
  for (int i_timezone = 0; i_timezone < sizeof(timezone_options) / sizeof(timezone_options[0]); i_timezone++) {
    if (timezone_options[i_timezone] == settings.timezone) {
      selected_timezone_i = i_timezone;
    }
  }
  if (M5.BtnA.wasPressed()) {
    selected_timezone_i = selected_timezone_i + 1;                                             // 1 is added to the value of selected_timezone_i
    if (selected_timezone_i > (sizeof(timezone_options) / sizeof(timezone_options[0])) - 1) {  // If the value of selected_timezone_i is equal to the number of indices in timezone_options
      selected_timezone_i = 0;                                                                 // selected_timezone_i returns to 0
    }
    settings.timezone = timezone_options[selected_timezone_i];  // The timezone will be the selected_timezone_i index of the timezone_options array
    draw_screen = true;
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Timezone");
    show_defined_configuration(settings.timezone);
    settings_footer("timezone");
  }
}

void screen_settings_date_format() {
  if (M5.BtnA.wasPressed()) {
    if (settings.date_format == "YYYY-MM-DD") {
      settings.date_format = "MM/DD/YYYY";
    } else if (settings.date_format == "MM/DD/YYYY") {
      settings.date_format = "DD/MM/YYYY";
    } else {
      settings.date_format = "YYYY-MM-DD";
    }
    draw_screen = true;
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Date Format");
    show_defined_configuration(settings.date_format);
    settings_footer("date format");
  }
}

void screen_settings_time_format() {
  if (M5.BtnA.wasPressed()) {
    toggle_time_format();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Time Format");
    show_defined_configuration(settings.time_format);
    settings_footer("time format");
  }
}

void screen_settings_baudrate() {
  if (M5.BtnA.wasPressed()) {
    set_baudrate();
    draw_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Baud Rate");
    M5.Display.setTextDatum(top_center);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Current baud Rate:", M5.Display.width() / 2, 35);
    M5.Display.setTextSize(2);
    M5.Display.drawString(String(settings.module_baudrate), M5.Display.width() / 2, 50);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Press button A to", M5.Display.width() / 2, 95);
    M5.Display.drawString("set a new baud rate.", M5.Display.width() / 2, 110);
  }
}

void screen_settings_reset_trip() {
  if (M5.BtnA.wasPressed()) {
    gps_trip.distance_from_start = 0;
    gps_trip.start_latitude = 0;
    gps_trip.start_longitude = 0;
    gps_trip.route_distance = 0;
    gps_trip.average_moving_speed = 0;
    gps_trip.route_last_latitude = 0;
    gps_trip.route_last_longitude = 0;
    gps_trip.moving_time_millis = 0;
    gps_trip.last_moving_millis = 0;
    gps_trip.max_speed = 0;
    gps_trip.min_speed = 0;
    gps_trip.max_altitude = 0;
    gps_trip.min_altitude = 0;
    gps_trip.trip_start_millis = 0;
    draw_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    if (gps_trip.start_latitude == 0 && gps_trip.max_speed == 0 && gps_trip.max_altitude == 0) {
      screen_header("Reset Trip");
      M5.Display.setTextDatum(top_center);
      M5.Display.setTextSize(2);
      M5.Display.drawString("Trip data reset!", M5.Display.width() / 2, 65);
    } else {
      screen_header("Reset Trip");
      M5.Display.setTextDatum(top_center);
      M5.Display.setTextSize(2);
      M5.Display.drawString("Reset Trip?", M5.Display.width() / 2, 35);
      M5.Display.setTextSize(1);
      M5.Display.drawString("All trip data will be reset.", M5.Display.width() / 2, 65);
      M5.Display.drawString("New data will be recorded", M5.Display.width() / 2, 80);
      M5.Display.drawString("from the next GPS/GNSS fix.", M5.Display.width() / 2, 95);
      M5.Display.setTextSize(1);
      M5.Display.drawString("Press A to confirm.", M5.Display.width() / 2, 120);
    }
  }
}

void screen_settings_exit() {
  if (M5.BtnA.wasPressed()) {
    settings_mode = false;
    toggle_screen = true;
    toggle_screen_settings = true;
    screen_i = 0;
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Exit");
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Press the A", M5.Display.width() / 2, 55);
    M5.Display.drawString("button to exit", M5.Display.width() / 2, 75);
    M5.Display.drawString("the settings", M5.Display.width() / 2, 95);
    M5.Display.setTextDatum(top_left);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(settings.theme_color);
    M5.Display.setCursor(10, 120);
    M5.Display.print("Button B = next setting");
  }
}

void show_defined_configuration(String defined_configuration) {
  M5.Display.fillRoundRect(20, 35, 200, 55, 10, settings.theme_color);
  M5.Display.setTextSize(3);
  M5.Display.setTextDatum(middle_center);
  M5.Display.setTextColor(BLACK);
  M5.Display.drawString(defined_configuration, M5.Display.width() / 2, 65);
}

void settings_footer(String settings_title) {
  M5.Display.setTextDatum(top_left);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(settings.theme_color);
  M5.Display.setCursor(10, 105);
  M5.Display.print("Button A = changes " + settings_title);
  M5.Display.setCursor(10, 120);
  M5.Display.print("Button B = next setting");
}

void screen_header(String title) {  // Function responsible for drawing the header box, the screen title, and the battery percentage, and clearing the information from the rest of the screen
  // Checks if the battery percentage, charging status, or screen has changed
  if (toggle_screen || toggle_screen_settings || data_comparison.battery_percentage != M5.Power.getBatteryLevel() || data_comparison.battery_charging != M5.Power.isCharging()) {
    M5.Display.fillRect(0, 0, 240, 25, settings.theme_color);                                             // Draw a square
    M5.Display.setTextSize(2);                                                                            // Setting the text size to 2
    M5.Display.setTextColor(BLACK);                                                                       // Setting the text color to black
    M5.Display.setTextDatum(top_left);                                                                    // Setting the text datum to top and left
    M5.Display.drawString(title, 10, 5);                                                                  // Print the title centered at the top
    M5.Display.setTextDatum(top_right);                                                                   // Setting the text datum to top and right
    if (M5.Power.isCharging() || data_comparison.battery_charging) {                                      // Check if it is charging
      M5.Display.drawString("+" + String(M5.Power.getBatteryLevel()) + "%", M5.Display.width() - 10, 5);  // Prints the + and the battery percentage.
    } else {                                                                                              // If it is not charging
      M5.Display.drawString(String(M5.Power.getBatteryLevel()) + "%", M5.Display.width() - 10, 5);        // prints the battery percentage
    }
    data_comparison.battery_percentage = M5.Power.getBatteryLevel();
    data_comparison.battery_charging = M5.Power.isCharging();
  }
  M5.Display.fillRect(0, 25, 240, 110, BLACK);  // Draw a black square
  // Check if the timeout warning screen should not be displayed and if the timeout is longer than acceptable
  if (system_state.display_timeout_warning == false && millis() - gps_time.last_update_millis >= (settings.gps_timeout_seconds * 1000)) {
    M5.Display.fillTriangle(235, 130, 214, 130, 225, 109, YELLOW);  // Draw a yellow triangle
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(BLACK);
    M5.Display.drawString("!", 226, 123);
    // Shows a warning icon when GNSS communication has timed out but the warning screen is not active
  }
  M5.Display.setTextColor(settings.theme_color);  // Setting the text color to settings.theme_color
  M5.Display.setTextDatum(top_left);              // Returning the text datum to the standard
}

void set_baudrate() {
  bool undefined_baudrate = true;                                                                                                           // Control variable for the repetition loop
  bool update_baudrate_display = true;                                                                                                      // Control variable for when the screen needs to be updated
  int selected_baudrate_i = findIndex(baudrate_options, sizeof(baudrate_options) / sizeof(baudrate_options[0]), settings.module_baudrate);  // Finding the already defined baudrate
  if (selected_baudrate_i < 0) {
    selected_baudrate_i = 0;
  }
  M5.Display.fillScreen(BLACK);                                        // Fill the screen with the color black
  M5.Display.setTextSize(2);                                           // Setting the text size to 2
  M5.Display.setTextColor(settings.theme_color);                       // Setting the text color to settings.theme_color
  M5.Display.setTextDatum(top_center);                                 // Setting the text datum to top and center
  M5.Display.drawString("Set Baud Rate", M5.Display.width() / 2, 10);  // "Set baud rate" is printed centered at the top
  M5.Display.setTextDatum(top_left);                                   // Setting the text datum to top and left
  while (undefined_baudrate) {                                         // It stays in this loop as long as undefined_baudrate is true
    M5.update();                                                       // Updates the state of the M5StickS3 hardware, especially buttons and input events
    if (M5.BtnB.wasPressed()) {                                        // Checks if button B has been pressed
      selected_baudrate_i = selected_baudrate_i + 1;                   // 1 is added to the value of selected_baudrate_i
      update_baudrate_display = true;                                  // update_baudrate_display is set to true to update the display
    }
    if (selected_baudrate_i > (sizeof(baudrate_options) / sizeof(baudrate_options[0])) - 1) {  // If the value of selected_baudrate_i is equal to the number of indices in baudrate_options
      selected_baudrate_i = 0;                                                                 // selected_baudrate_i returns to 0
    }
    if (update_baudrate_display) {
      M5.Display.fillRect(0, 40, 240, 95, BLACK);                                                                    // Draw a square on the screen
      M5.Display.setCursor(0, 45);                                                                                   // Moves the cursor to the specified X (0) and Y (45) coordinates
      for (int i_baudrate = 0; i_baudrate < sizeof(baudrate_options) / sizeof(baudrate_options[0]); i_baudrate++) {  // traverses the baudrate_options array
        if (selected_baudrate_i == i_baudrate) {                                                                     // If selected_baudrate_i is equal to i_baudrate
          M5.Display.print(" > ");                                                                                   // print >
          M5.Display.println(baudrate_options[i_baudrate]);                                                          // prints the baudrate of the matched index
        } else {                                                                                                     // If selected_baudrate_i is not equal to i_baudrate
          M5.Display.print("   ");                                                                                   // prints a space
          M5.Display.println(baudrate_options[i_baudrate]);                                                          // prints the baudrate of the matched index
        }
      }
      update_baudrate_display = false;  // update_baudrate_display is set to false to avoid unnecessary display updates
    }
    if (M5.BtnA.wasPressed()) {                                          // Checks if button A has been pressed
      settings.module_baudrate = baudrate_options[selected_baudrate_i];  // The module_baudrate will be the selected_baudrate_i index of the baudrate_options array
      undefined_baudrate = false;                                        // undefined_baudrate is set to false to exit the loop
      save_settings();
      if (gps_serial_initialized) {  // Checks if a serial communication has already been initiated.
        gps_serial.end();            // Closes the current UART configuration before applying the new baud rate
      }
      gps_serial.begin(
          settings.module_baudrate,
          SERIAL_8N1,
          GPS_RX_PIN,
          GPS_TX_PIN);                // Starts a new serial communication with the specified baud rate
      gps_serial_initialized = true;  // Sets gps_serial_initialized to true. If another serial communication needs to be initiated, the variable indicates that the previous one must be terminated
      M5.Display.fillScreen(BLACK);   // Fill the screen with the color black
      M5.Display.setCursor(0, 35);    // Moves the cursor to the specified X (0) and Y (35) coordinates
      M5.Display.println("  Baud Rate set to: ");
      M5.Display.println("");                        // Skip to the next line
      M5.Display.print("  ");                        // prints a space
      M5.Display.setTextSize(3);                     // Setting the text size to 3
      M5.Display.println(settings.module_baudrate);  // Prints the selected baud rate value
      delay(1500);                                   // Waiting time of 1500 milliseconds
    }
  }
}

int findIndex(int array[], int size, int value) {  // Function to find the index of an array by value
  for (int i = 0; i < size; i++) {                 // traverses the array
    if (array[i] == value) {                       // If the index value is equal to the searched value
      return i;                                    // Returns the index
    }
  }
  return -1;  // Returns -1 if not found
}

void toggle_coord_format() {
  if (M5.BtnA.wasPressed()) {
    if (settings.coord_format == "Decimal") {
      settings.coord_format = "DMS";
    } else if (settings.coord_format == "DMS") {
      settings.coord_format = "DM";
    } else {
      settings.coord_format = "Decimal";
    }
    draw_screen = true;
    save_settings();
  }
}

void toggle_speed_unit() {
  byte selected_unit_i = 0;  // Finding the already defined speed unit
  for (int i_unit = 0; i_unit < sizeof(units_speed_measurement_options) / sizeof(units_speed_measurement_options[0]); i_unit++) {
    if (units_speed_measurement_options[i_unit] == settings.units_speed_measurement) {
      selected_unit_i = i_unit;
    }
  }
  if (M5.BtnA.wasPressed()) {
    selected_unit_i = selected_unit_i + 1;                                                                               // 1 is added to the value of selected_unit_i
    if (selected_unit_i > (sizeof(units_speed_measurement_options) / sizeof(units_speed_measurement_options[0])) - 1) {  // If the value of selected_unit_i is equal to the number of indices in units_speed_measurement_options
      selected_unit_i = 0;                                                                                               // selected_unit_i returns to 0
    }
    settings.units_speed_measurement = units_speed_measurement_options[selected_unit_i];  // The units_speed_measurement will be the selected_unit_i index of the units_speed_measurement_options array
    draw_screen = true;
    data_comparison.speed = data_comparison.speed + 1;
    save_settings();
  }
}

String convert_speed(double speed, bool show_unit) {  // Function to convert speed
  double converted_speed;
  if (settings.units_speed_measurement == "km/h") {
    converted_speed = speed;
  } else if (settings.units_speed_measurement == "mph") {
    converted_speed = speed * 0.621371;
  } else if (settings.units_speed_measurement == "knots") {
    converted_speed = speed * 0.539957;
  } else if (settings.units_speed_measurement == "m/s") {
    converted_speed = speed / 3.6;
  } else {
    converted_speed = speed;
  }
  String result = String((int)converted_speed);
  result.trim();
  if (show_unit) {
    result += " " + settings.units_speed_measurement;
  }
  return result;
}

void toggle_altitude_unit() {
  if (M5.BtnA.wasPressed()) {
    if (settings.unit_altitude_measurement == "Meters") {
      settings.unit_altitude_measurement = "Feet";
    } else {
      settings.unit_altitude_measurement = "Meters";
    }
    draw_screen = true;
    data_comparison.altitude = data_comparison.altitude + 1;
    save_settings();
  }
}

void toggle_distance_unit() {
  if (M5.BtnA.wasPressed()) {
    if (settings.unit_distance_measurement == "Metric") {
      settings.unit_distance_measurement = "Imperial";
    } else {
      settings.unit_distance_measurement = "Metric";
    }
    draw_screen = true;
    save_settings();
  }
}

void toggle_time_format() {
  if (settings.time_format == "24-hour") {
    settings.time_format = "12-hour";
  } else {
    settings.time_format = "24-hour";
  }
  draw_screen = true;
  save_settings();
}

String get_elapsed_time(unsigned long start_millis) {                // Takes the start time in millis() and returns the elapsed time
  unsigned long elapsed_seconds = (millis() - start_millis) / 1000;  // Subtracts the current millis() and converts it to seconds
  unsigned long hours = elapsed_seconds / 3600;                      // Gets the elapsed hours
  unsigned long minutes = (elapsed_seconds % 3600) / 60;             // Gets the elapsed minutes
  unsigned long seconds = elapsed_seconds % 60;                      // Gets the elapsed seconds
  if (hours > 0) {                                                   // If more than an hour has passed
    return String(hours) + " h " + String(minutes) + " min";         // Returns the elapsed hours and minutes
  } else if (minutes > 0) {                                          // If more than a minute has passed
    return String(minutes) + " min " + String(seconds) + " s";       // Returns the elapsed minutes and seconds
  } else {                                                           // If the elapsed time is less than one minute
    return String(seconds) + " s";                                   // Returns the elapsed time in seconds
  }
}

String millis_to_time(unsigned long start_millis) {             // Converts the time received in millis() into hours, minutes, and seconds.
  unsigned long elapsed_seconds = start_millis / 1000;          // Subtracts the current millis() and converts it to seconds
  unsigned long hours = elapsed_seconds / 3600;                 // Gets the elapsed hours
  unsigned long minutes = (elapsed_seconds % 3600) / 60;        // Gets the elapsed minutes
  unsigned long seconds = elapsed_seconds % 60;                 // Gets the elapsed seconds
  if (hours > 0) {                                              // If more than an hour has passed
    return String(hours) + " h " + String(minutes) + " min";    // Returns the elapsed hours and minutes
  } else if (minutes > 0) {                                     // If more than a minute has passed
    return String(minutes) + " min " + String(seconds) + " s";  // Returns the elapsed minutes and seconds
  } else {                                                      // If the elapsed time is less than one minute
    return String(seconds) + " s";                              // Returns the elapsed time in seconds
  }
}

void equalize_comparison() {
  data_comparison.latitude = gps_data.latitude;
  data_comparison.longitude = gps_data.longitude;
  data_comparison.altitude = gps_data.altitude;
  data_comparison.speed = gps_data.speed;
  data_comparison.course = gps_data.course;
  data_comparison.satellites = gps_data.satellites;
  data_comparison.hdop = gps_data.hdop;
  data_comparison.hdop_quality = gps_data.hdop_quality;
}

void update_date_time() {                                                                 // Converts UTC GNSS time to the selected timezone and adjusts the date when crossing midnight
  int sign = (settings.timezone.charAt(3) == '+') ? 1 : -1;                               // Extract timezone sign
  int timezone_hour = settings.timezone.substring(4, 6).toInt();                          // Extract timezone hours
  int timezone_minute = settings.timezone.substring(7, 9).toInt();                        // Extract timezone minutes
  int timezone_seconds = sign * ((timezone_hour * 3600) + (timezone_minute * 60));        // Convert timezone to seconds
  int total_seconds = (gps_data.hour * 3600) + (gps_data.minute * 60) + gps_data.second;  // Convert GPS time to seconds
  total_seconds += timezone_seconds;                                                      // Apply timezone
  gps_time.year = gps_data.year;
  gps_time.month = gps_data.month;
  gps_time.day = gps_data.day;
  // Adjust date if the timezone caused a day change
  if (total_seconds < 0) {
    total_seconds += 86400;
    gps_time.day--;
    if (gps_time.day < 1) {
      gps_time.month--;
      if (gps_time.month < 1) {
        gps_time.month = 12;
        gps_time.year--;
      }
      // Number of days in the previous month
      if (gps_time.month == 2) {
        bool leap_year = (gps_time.year % 4 == 0 && (gps_time.year % 100 != 0 || gps_time.year % 400 == 0));
        gps_time.day = leap_year ? 29 : 28;
      } else if (gps_time.month == 4 || gps_time.month == 6 || gps_time.month == 9 || gps_time.month == 11) {
        gps_time.day = 30;
      } else {
        gps_time.day = 31;
      }
    }
  } else if (total_seconds >= 86400) {  // Adjusts the date when applying the timezone moves the time to the previous or next day
    total_seconds -= 86400;
    gps_time.day++;
    int days_in_month;  // Number of days in the current month
    if (gps_time.month == 2) {
      bool leap_year = (gps_time.year % 4 == 0 && (gps_time.year % 100 != 0 || gps_time.year % 400 == 0));
      days_in_month = leap_year ? 29 : 28;
    } else if (gps_time.month == 4 || gps_time.month == 6 || gps_time.month == 9 || gps_time.month == 11) {
      days_in_month = 30;
    } else {
      days_in_month = 31;
    }
    if (gps_time.day > days_in_month) {
      gps_time.day = 1;
      gps_time.month++;

      if (gps_time.month > 12) {
        gps_time.month = 1;
        gps_time.year++;
      }
    }
  }
  // Convert seconds back to time
  gps_time.hour = total_seconds / 3600;
  gps_time.minute = (total_seconds % 3600) / 60;
  gps_time.second = total_seconds % 60;
}

byte percentage_to_byte(byte percentage) {  // Converts 0–100 to 0–255.
  percentage = constrain(percentage, 0, 100);
  return (percentage * 255) / 100;
}

void set_default_settings() {  // Sets the default firmware configuration values
  settings.module_baudrate = baudrate_options[0];
  settings.theme_color = theme_color_options[0];
  settings.coord_format = "Decimal";
  settings.units_speed_measurement = units_speed_measurement_options[0];
  settings.unit_altitude_measurement = "Meters";
  settings.unit_distance_measurement = "Metric";
  settings.timezone = timezone_options[14];
  settings.time_format = "24-hour";
  settings.date_format = "YYYY-MM-DD";
  settings.screen_brightness = 100;
  settings.screen_timeout_seconds = screen_timeout_options[0];
  settings.gps_timeout_seconds = 15;
  settings.min_moving_speed = 1.0;
}

void save_settings() {  // Saves the current configuration to NVS so it survives reboot and power loss
  preferences.begin(SETTINGS_NAMESPACE, false);
  preferences.putBool("initialized", true);
  preferences.putInt("baudrate", settings.module_baudrate);
  preferences.putUShort("theme", settings.theme_color);
  preferences.putString("coord", settings.coord_format);
  preferences.putString("speed_unit", settings.units_speed_measurement);
  preferences.putString("altitude_unit", settings.unit_altitude_measurement);
  preferences.putString("distance_unit", settings.unit_distance_measurement);
  preferences.putString("timezone", settings.timezone);
  preferences.putString("time_format", settings.time_format);
  preferences.putString("date_format", settings.date_format);
  preferences.putUChar("brightness", settings.screen_brightness);
  preferences.putInt("screen_timeout", settings.screen_timeout_seconds);
  preferences.putInt("gps_timeout", settings.gps_timeout_seconds);
  preferences.putDouble("min_speed", settings.min_moving_speed);
  preferences.end();
}

void load_settings() {                                           // Loads the saved firmware settings from persistent storage
  preferences.begin(SETTINGS_NAMESPACE, true);                   // Opens the settings namespace in read-only mode
  bool initialized = preferences.getBool("initialized", false);  // Checks whether saved settings exist, using false as the default value
  if (!initialized) {                                            // Checks if the settings have not been initialized yet
    // No persistent settings were found, so initialize defaults and require the user to select the GNSS baud rate
    preferences.end();       // Closes the Preferences namespace
    set_default_settings();  // Loads the default firmware settings
    set_baudrate();          // Calls the function to set the baudrate
    save_settings();         // Saves the settings
    return;
  }
  settings.module_baudrate = preferences.getInt("baudrate", baudrate_options[0]);
  settings.theme_color = preferences.getUShort("theme", theme_color_options[0]);
  settings.coord_format = preferences.getString("coord", "Decimal");
  settings.units_speed_measurement = preferences.getString("speed_unit", units_speed_measurement_options[0]);
  settings.unit_altitude_measurement = preferences.getString("altitude_unit", "Meters");
  settings.unit_distance_measurement = preferences.getString("distance_unit", "Metric");
  settings.timezone = preferences.getString("timezone", timezone_options[14]);
  settings.time_format = preferences.getString("time_format", "24-hour");
  settings.date_format = preferences.getString("date_format", "YYYY-MM-DD");
  settings.screen_brightness = preferences.getUChar("brightness", 100);
  settings.screen_timeout_seconds = preferences.getInt("screen_timeout", screen_timeout_options[0]);
  settings.gps_timeout_seconds = preferences.getInt("gps_timeout", 15);
  settings.min_moving_speed = preferences.getDouble("min_speed", 1.0);
  preferences.end();
}

bool validate_settings() {     // Validates settings loaded from NVS and replaces invalid values with safe defaults
  bool settings_valid = true;  // Returns true when all settings are valid, otherwise false after correcting invalid values
  bool valid_baudrate = false;
  for (int i = 0;
       i < sizeof(baudrate_options) / sizeof(baudrate_options[0]);
       i++) {
    if (settings.module_baudrate == baudrate_options[i]) {
      valid_baudrate = true;
      break;
    }
  }
  if (!valid_baudrate) {
    settings.module_baudrate = baudrate_options[0];
    settings_valid = false;
  }
  bool valid_theme_color = false;
  for (int i = 0;
       i < sizeof(theme_color_options) / sizeof(theme_color_options[0]);
       i++) {
    if (settings.theme_color == theme_color_options[i]) {
      valid_theme_color = true;
      break;
    }
  }
  if (!valid_theme_color) {
    settings.theme_color = theme_color_options[0];
    settings_valid = false;
  }
  if (settings.coord_format != "Decimal" &&
      settings.coord_format != "DMS" &&
      settings.coord_format != "DM") {
    settings.coord_format = "Decimal";
    settings_valid = false;
  }
  if (settings.units_speed_measurement != "km/h" &&
      settings.units_speed_measurement != "mph" &&
      settings.units_speed_measurement != "knots" &&
      settings.units_speed_measurement != "m/s") {
    settings.units_speed_measurement = units_speed_measurement_options[0];
    settings_valid = false;
  }
  if (settings.unit_altitude_measurement != "Meters" &&
      settings.unit_altitude_measurement != "Feet") {
    settings.unit_altitude_measurement = "Meters";
    settings_valid = false;
  }
  if (settings.unit_distance_measurement != "Metric" &&
      settings.unit_distance_measurement != "Imperial") {
    settings.unit_distance_measurement = "Metric";
    settings_valid = false;
  }
  bool valid_timezone = false;
  for (int i = 0;
       i < sizeof(timezone_options) / sizeof(timezone_options[0]);
       i++) {
    if (settings.timezone == timezone_options[i]) {
      valid_timezone = true;
      break;
    }
  }
  if (!valid_timezone) {
    settings.timezone = timezone_options[14];
    settings_valid = false;
  }
  if (settings.time_format != "24-hour" &&
      settings.time_format != "12-hour") {
    settings.time_format = "24-hour";
    settings_valid = false;
  }
  if (settings.date_format != "YYYY-MM-DD" &&
      settings.date_format != "MM/DD/YYYY" &&
      settings.date_format != "DD/MM/YYYY") {
    settings.date_format = "YYYY-MM-DD";
    settings_valid = false;
  }
  if (settings.screen_brightness < 10 ||
      settings.screen_brightness > 100 ||
      settings.screen_brightness % 10 != 0) {
    settings.screen_brightness = 100;
    settings_valid = false;
  }
  bool valid_screen_timeout = false;
  for (int i = 0;
       i < sizeof(screen_timeout_options) / sizeof(screen_timeout_options[0]);
       i++) {
    if (settings.screen_timeout_seconds == screen_timeout_options[i]) {
      valid_screen_timeout = true;
      break;
    }
  }
  if (!valid_screen_timeout) {
    settings.screen_timeout_seconds = screen_timeout_options[0];
    settings_valid = false;
  }
  if (settings.gps_timeout_seconds < 1 ||
      settings.gps_timeout_seconds > 300) {
    settings.gps_timeout_seconds = 15;
    settings_valid = false;
  }
  if (settings.min_moving_speed < 0.0 ||
      settings.min_moving_speed > 100.0) {
    settings.min_moving_speed = 1.0;
    settings_valid = false;
  }
  return settings_valid;
}