// GPS & GNSS Monitor
// Firmware Version: 1.2

// GPS & GNSS Monitor is a firmware for the M5StickS3 that receives and displays
// real-time data from a connected GPS/GNSS module. It provides multiple screens
// for monitoring position, altitude, speed, course, satellite reception, HDOP,
// date, and time, as well as trip and route statistics. The firmware supports
// configurable units, coordinate and time formats, timezone, display settings,
// UART pins and baud rate, with settings saved persistently in NVS.

// Developed by ViniciusHNF
// https://github.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor
// https://youtube.com/@ViniciusHNF

// IMPORTANT:
// This firmware requires the M5StickS3 board support package and the required libraries
// to be installed in the Arduino IDE before compiling and uploading.
// For instructions on configuring the M5StickS3 in Arduino IDE, visit:
// https://docs.m5stack.com/en/arduino/m5sticks3/program

// For the M5StickS3, the recommended GPIO configuration depends on the connection interface.
// Hat2-Bus: Set G44 as RX and G43 as TX. Connect the GPS/GNSS module's TX -> G44 and RX -> G33.
// HY2.0-4P: Set G10 as RX and G9 as TX. Connect the GPS/GNSS module's TX -> G10 and RX -> G9.

#define VERSION_FIRMWARE 1.2  // Firmware Version

#include <HardwareSerial.h>  // Library for UART serial communication
#include <M5Unified.h>       // Library for controlling M5Stack hardware
#include <Preferences.h>     // Library for storing persistent settings in NVS
#include <TinyGPSPlus.h>     // Library for parsing GPS/GNSS NMEA data

unsigned long button_b_press_time = 0;  // Variable to store the time when button B is pressed
bool button_b_pressed = false;          // Variable that checks if button B is being pressed
int time_long_press_button_b = 300;     // Variable that stores the duration for which button B must be held down to be considered a long press

byte screen_i = 0;                          // Variable to control which screen will be displayed
bool draw_screen = true;                    // Variable to control when the screen needs to be redrawn
bool toggle_screen = true;                  // Variable to control when the screen has been toggled
bool refresh_screen_exit_settings = false;  // Forces the screen to update after exiting the settings. This variable is necessary because the draw_screen and toggle_screen variables are reset to false at the end of the loop

bool toggle_screen_settings = true;  // Variable to control when the settings screen has been toggled
bool settings_mode = false;          // Variable to control when the settings mode is active
byte screen_settings_i = 0;          // Variable to control which settings screen will be displayed
byte screen_general_i = 0;           // Variable to control which general screen will be displayed
byte screen_route_i = 0;             // Variable to control which route statistics screen will be displayed
bool screen_about_i = 0;             // Variable to control which about screen will be displayed

int gpio_options[] = {1, 2, 9, 10, 13, 15, 26, 32, 33, 36, 43, 44};          // Array of GPIO options that can be TX or RX
int baudrate_options[] = {9600, 19200, 38400, 57600, 115200};                // Array of baud rates
String units_speed_measurement_options[] = {"km/h", "mph", "knots", "m/s"};  // Array of available speed measurement units
int screen_timeout_options[] = {0, 10, 30, 60, 300, 600};                    // Screen timeout values in seconds
// Array of available theme colors
uint16_t theme_color_options[] = {WHITE, BLACK, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, 0xFDA0, 0x780F, 0xFE19, 0x000F, 0x9A60, 0x8410};
// Array of available theme colors in string format
String theme_color_options_string[] = {"WHITE", "BLACK", "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA", "ORANGE", "PURPLE", "PINK", "NAVY", "BROWN", "GRAY"};
// Array of available timezones
String timezone_options[] = {"UTC-12:00", "UTC-11:00", "UTC-10:00", "UTC-09:30", "UTC-09:00", "UTC-08:00", "UTC-07:00", "UTC-06:00", "UTC-05:00", "UTC-04:00", "UTC-03:30", "UTC-03:00", "UTC-02:00", "UTC-01:00", "UTC+00:00", "UTC+01:00", "UTC+02:00", "UTC+03:00", "UTC+03:30", "UTC+04:00", "UTC+04:30", "UTC+05:00", "UTC+05:30", "UTC+05:45", "UTC+06:00", "UTC+06:30", "UTC+07:00", "UTC+08:00", "UTC+08:45", "UTC+09:00", "UTC+09:30", "UTC+10:00", "UTC+10:30", "UTC+11:00", "UTC+12:00", "UTC+12:45", "UTC+13:00", "UTC+14:00"};

bool new_gps_position = false;        // Indicates that a new valid GPS/GNSS position was received and trip/route data must be updated
bool gps_serial_initialized = false;  // Variable that stores information on whether serial communication has already been initiated. Prevents calling end() on the UART before it has been initialized
double last_speed_course = 0;         // Variable that stores the last speed value displayed on the course screen to determine whether the screen needs to be redrawn and the speed reset to 0
byte horizontal_screen_rotation = 3;  // Stores the rotation value for horizontal screens
byte vertical_screen_rotation = 0;    // Stores the rotation value for vertical screens

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
  unsigned long trip_start_millis;                  // Session start timestamp, in milliseconds since the device started
  unsigned long last_millis_shown_info;             // Timestamp of the last displayed session information update
  unsigned long time_in_motion_millis;              // Total accumulated time spent in motion, in milliseconds
  unsigned long last_time_in_motion_update_millis;  // Timestamp of the last time-in-motion counter update
  unsigned long last_speed_update_millis = 0;       // Timestamp of the last valid GPS/GNSS speed update
  bool is_moving = false;                           // Indicates whether the latest valid speed measurement meets the minimum movement threshold
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
  unsigned long last_screen_update_millis;          // Stores the millis() timestamp of the last screen update, used to determine when the screen should update again based on settings.screen_update_interval_seconds
  bool screen_on;                                   // Stores information on whether the screen is on
  bool first_loop;                                  // Stores the information from the first loop
};

struct struct_settings {               // Structure for storing settings
  int module_baudrate;                 // Selected Baud rate
  int gpio_rx;                         // GPIO selected to receive data for the GPS/GNSS module
  int gpio_tx;                         // GPIO selected to transmit data to the GPS/GNSS module
  uint16_t theme_color;                // Variable to control the theme color of the application
  uint16_t background_color;           // Variable to control the background color of the application
  String coord_format;                 // Variable for controlling the coordinate format. Decimal || DMS || DM
  String units_speed_measurement;      // Variable to control the speed measurement unit of the application
  String unit_altitude_measurement;    // Variable to control the altitude measurement unit of the application. Meters || Feet
  String unit_distance_measurement;    // Variable for controlling the distance unit of measurement. Metric || Imperial
  String timezone;                     // Variable that stores the selected timezone
  String time_format;                  // Variable to control the time format. 24-hour || 12-hour
  String date_format;                  // Variable to control the date format. YYYY-MM-DD || MM/DD/YYYY || DD/MM/YYYY
  byte screen_brightness;              // Screen brightness percentage. 100 = 100% brightness
  int screen_timeout_seconds;          // Time in seconds for the screen to turn off when there is no button interaction
  int gps_timeout_seconds;             // Time in seconds to consider that the GPS/GNSS module has not sent new information for a long time
  int screen_update_interval_seconds;  // Screen update interval in seconds. Set to 0 for automatic updates whenever new GPS/GNSS data is received
  double min_moving_speed;             // Minimum GPS/GNSS speed in km/h required to record route distance and moving time
  bool flip_screen_horizontally;       // Rotates horizontal screens by 180° when enabled
  bool flip_screen_vertically;         // Rotates vertical screens by 180° when enabled
  bool rotate_screen_90_degrees;       // Rotates the screen by +90° when enabled
};

struct_gps_data gps_data = {};                // Stores the GPS/GNSS data received from the module, including position, altitude, speed, course, satellite count, HDOP, and date and time
struct_gps_TripData gps_trip = {};            // Stores trip and route information, including traveled distances, starting position, average moving speed, movement time, and minimum and maximum values
struct_gps_time gps_time = {};                // Stores the date and time obtained from the GPS/GNSS module, including the first position fix timestamp and the timestamp of the last update
struct_data_comparison data_comparison = {};  // Stores previous or reference values of GPS/GNSS data, along with additional display and battery information, for comparing changes in system data
struct_settings settings = {};                // Stores the user's configuration preferences, including communication settings, GPIO pins, display appearance, units of measurement, time zone, and timeouts
struct_system_state system_state = {};        // Stores temporary system states and timestamps used to manage display updates, screen timeout warnings, button interactions, and screen power status

TinyGPSPlus gps;                          // Creates the TinyGPSPlus object for parsing GPS/GNSS data
HardwareSerial gps_serial(1);             // Creates a hardware UART serial interface on UART1
Preferences preferences;                  // Creates the Preferences object for persistent settings storage
#define SETTINGS_NAMESPACE "gpssettings"  // Defines the namespace used to store firmware settings in NVS

void setup() {
  M5.begin();                                                                                              // Initializes the M5 library
  M5.Display.setBrightness(percentage_to_byte(100));                                                       // Sets the splash screen brightness to 100%
  M5.Display.fillScreen(BLACK);                                                                            // Fill the screen with the color black
  M5.Display.setRotation(horizontal_screen_rotation);                                                      // Sets the display rotation to 270 degrees (rotation mode 3), adjusting the screen orientation to match the desired layout
  M5.Display.setTextSize(3);                                                                               // Setting the text size to 3
  M5.Display.setTextColor(WHITE);                                                                          // Setting the text color to WHITE
  M5.Display.setTextDatum(top_center);                                                                     // Setting the text datum to top and center
  M5.Display.drawString("GPS & GNSS", M5.Display.width() / 2, 20);                                         // Print "GPS and GNSS" centered horizontally at Y = 20
  M5.Display.drawString("Monitor", M5.Display.width() / 2, 55);                                            // Print "Monitor" centered horizontally at Y = 55
  M5.Display.setTextSize(1);                                                                               // Setting the text size to 2
  M5.Display.drawString("Firmware Version: " + String(VERSION_FIRMWARE, 1), M5.Display.width() / 2, 100);  // Prints the firmware version
  M5.Display.drawString("github.com/viniciushnf", M5.Display.width() / 2, 120);                            // Print the GitHub link
  M5.Display.setTextDatum(top_left);                                                                       // Returning the text datum to the standard
  delay(3000);                                                                                             // Waiting time of 3000 milliseconds (3 seconds)

  load_settings();             // Function that retrieves persistent settings and saves them to variables
  if (!validate_settings()) {  // Checks if the settings are valid
    basic_settings();          // Calls the function that defines the initial settings: baud rate, GPIO RX, and GPIO TX
    save_settings();           // If any configuration is invalid, save the settings with valid parameters
  }

  apply_rotation();                                                          // Applies screen rotation and flip settings
  M5.Display.setTextColor(settings.theme_color);                             // Setting the text color to settings.theme_color
  M5.Display.setBrightness(percentage_to_byte(settings.screen_brightness));  // Set the screen brightness based on the percentage value
  system_state.screen_on = true;                                             // Sets system_state.screen_on to true, which means the screen is on

  if (gps_serial_initialized) {  // Checks if a serial communication has already been initiated
    gps_serial.end();            // Closes the current UART configuration before applying the new baud rate
  }
  gps_serial.begin(              // Initializes the UART to be used by the GPS/GNSS
      settings.module_baudrate,  // Baud rate
      SERIAL_8N1,                // Configuration: 8 bits, no parity, 1 stop bit
      settings.gpio_rx,          // GPIO RX pin
      settings.gpio_tx           // GPIO TX pin
  );
  gps_serial_initialized = true;  // Sets gps_serial_initialized to true. If another serial communication needs to be initiated, the variable indicates that the previous one must be terminated

  gps_data.hdop_quality = "No Fix";                        // The HDOP quality is set to "No Fix"
  system_state.display_timeout_warning = false;            // system_state.display_timeout_warning is set to false
  system_state.last_button_interaction_millis = millis();  // Stores the value of millis() so that the screen timeout only starts counting at the end of setup()
  system_state.first_loop = true;                          // Indicates that this is the first time the firmware will execute the loop
}

void loop() {
  if (gps_serial.available() > 0) {  // Checks for new data from the GPS/GNSS module
    update_gps_data();               // Updates GPS data
    if (new_gps_position) {          // Checks for a new position
      update_gps_trip();             // Updates trip data such as distance from the starting point and minimum/maximum values
      update_gps_route();            // Updates route statistics such as traveled distance and average moving speed
      new_gps_position = false;      // Sets new_gps_position to false. This ensures that the firmware only updates the Trip and Route when new information is available from the GPS/GNSS module
    }
  }
  update_time_in_motion();  // Function that updates the time spent in motion

  if (settings.screen_update_interval_seconds != 0) {                                                             // Check that the screen update interval is not set to automatic
    if ((settings.screen_update_interval_seconds * 1000) >= millis() - system_state.last_screen_update_millis) {  // Converts the screen update interval into milliseconds and checks whether it is equal to or greater than the time that has elapsed since the last update
      draw_screen = false;                                                                                        // Set draw_screen to false. This ensures that new information from the GPS/GNSS module does not update the screen
    } else {                                                                                                      // If the time interval is greater than the screen update interval
      draw_screen = true;                                                                                         // Forces a screen refresh
    }
    // Switching between screens and settings triggers an automatic screen refresh, regardless of the screen update interval
  }

  if (system_state.first_loop) {      // Check if it is the first loop
    draw_screen = true;               // Forces the screen to refresh
    system_state.first_loop = false;  // Indicates that this is no longer the first loop, so there is no need to force a screen refresh as would be required for the initial loop
  }

  M5.update();  // Updates the state of the M5StickS3 hardware, especially buttons and input events

  if (M5.BtnB.wasPressed()) {        // Checks if button B was pressed.
    button_b_press_time = millis();  // button_b_press_time receives the value of millis(). millis() returns the time in milliseconds since the ESP32 was powered on.
    button_b_pressed = true;         // button_b_pressed is set to true
  }

  if (settings_mode == false && (system_state.display_timeout_warning == false || settings.gps_timeout_seconds == 0)) {  // If configuration mode is disabled and if the timeout warning is not displayed
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
      // If the current screen is the General screen and the general screen index is greater than 0, or if the current screen is the Route Statistics screen and the route statistics screen index is greater than 0
      if ((screen_i == 0 && screen_general_i > 0) || (screen_i == 5 && screen_route_i > 0)) {
        M5.Display.setRotation(vertical_screen_rotation);  // Vertical screen (with button A at the bottom or top)
      } else {
        M5.Display.setRotation(horizontal_screen_rotation);  // Standard rotation (screen in landscape orientation, with button A on the left or the right)
      }
    }
  }

  // Forces a screen refresh when exiting settings or the communication notice
  if (refresh_screen_exit_settings) {  // Check whether you have exited the settings (or the communication notice)
    // Forces a screen refresh within this loop, even without the need to press a button or receive a new update from the GPS/GNSS module
    draw_screen = true;                    // Forces the screen to refresh
    toggle_screen = true;                  // Forces the screen to refresh
    refresh_screen_exit_settings = false;  // Set to false to avoid updating the screen during each iteration of the loop
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

  if (settings.gps_timeout_seconds != 0 && system_state.display_timeout_warning) {  // Whether the warning should be displayed on the screen
    // The screen must be redrawn every second to show the duration of the communication issues
    if (millis() - system_state.last_millis_shown_warning_timeout > 1000) {  // Checks whether the time interval is greater than 1 second
      system_state.last_millis_shown_warning_timeout = millis();             // Updates the variable with the current millis()
      draw_screen = true;                                                    // Indicates that the screen needs to be updated
    }
    if (draw_screen) {
      M5.Display.setRotation(horizontal_screen_rotation);  // Standard rotation (screen in landscape orientation, with button A on the left or the right)
      screen_header("Warning");                            // Draws the header with the title "Warning"
      M5.Display.setTextSize(2);
      M5.Display.setTextDatum(top_center);
      M5.Display.drawString("Communication", M5.Display.width() / 2, 35);
      M5.Display.drawString("Warning", M5.Display.width() / 2, 55);
      M5.Display.setTextSize(1);
      M5.Display.drawString("No updates received for " + millis_to_time(millis() - gps_time.last_update_millis), M5.Display.width() / 2, 90);  // Shows the elapsed time
      M5.Display.drawString("Press Button A to dismiss", M5.Display.width() / 2, 115);
      draw_screen = false;  // draw_screen is set to false so the screen isn't drawn every time
      if ((screen_i == 0 && screen_general_i > 0) || (screen_i == 5 && screen_route_i > 0)) {
        // If the current screen is the General screen and the general screen index is greater than 0, or if the current screen is the Route Statistics screen and the route statistics screen index is greater than 0
        M5.Display.setRotation(vertical_screen_rotation);  // Vertical screen (with button A at the bottom or top)
      }
    }
    if (M5.BtnA.wasPressed()) {                      // If button A was pressed
      system_state.display_timeout_warning = false;  // Dismiss the warning
      draw_screen = true;                            // Forces the screen to refresh
      toggle_screen = true;                          // Forces the screen to refresh
      refresh_screen_exit_settings = true;           // // Forces the screen to refresh on the next loop, even without the need to press a button or receive a new update from the GPS/GNSS module
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
    gps_data.latitude = gps.location.lat();                  // Updates the latitude
    gps_data.longitude = gps.location.lng();                 // Updates the longitude
    gps_time.last_update_millis = millis();                  // Stores the millis() value where the last valid information was received
    new_gps_position = true;                                 // Indicates that there is a new position
    if (gps_time.first_fix_millis == 0) {                    // Checks whether a valid position did not exist
      gps_time.first_fix_millis = millis();                  // Stores the time of the first valid position fix since startup
    }
  }
  if (gps.altitude.isUpdated() && gps.altitude.isValid()) {  // Altitude
    gps_data.altitude = gps.altitude.meters();               // Updates the altitude in meters
    gps_time.last_update_millis = millis();                  // Stores the millis() value where the last valid information was received
  }
  if (gps.speed.isUpdated() && gps.speed.isValid()) {                  // Check if the speed has updated and if it is valid
    gps_data.speed = gps.speed.kmph();                                 // Stores the current speed in kilometers per hour
    gps_trip.last_speed_update_millis = millis();                      // Records the timestamp of the latest valid speed update
    gps_trip.is_moving = gps_data.speed >= settings.min_moving_speed;  // Determines whether the current speed meets the minimum movement threshold
  }
  if (gps.course.isUpdated() && gps.course.isValid()) {  // Course
    gps_data.course = gps.course.deg();                  // Update the course in degrees
  }
  if (gps.satellites.isUpdated() && gps.satellites.isValid()) {  // Satellites
    gps_data.satellites = gps.satellites.value();                // Updates the number of satellites
    gps_time.last_update_millis = millis();                      // Stores the millis() value where the last valid information was received
  }
  if (gps.hdop.isUpdated() && gps.hdop.isValid()) {  // HDOP
    gps_data.hdop = gps.hdop.hdop();                 // Updates the HDOP
  }
  if (gps.date.isUpdated() && gps.date.isValid()) {  // Date
    gps_data.year = gps.date.year();                 // Year
    gps_data.month = gps.date.month();               // Month
    gps_data.day = gps.date.day();                   // Day
    update_date_time();                              // Function that updates the time according to the time zone
  }
  if (gps.time.isUpdated() && gps.time.isValid()) {  // Time
    gps_data.hour = gps.time.hour();                 // Hour
    gps_data.minute = gps.time.minute();             // Minute
    gps_data.second = gps.time.second();             // Second
    update_date_time();                              // Function that updates the time according to the time zone
    gps_time.last_update_millis = millis();          // Stores the millis() value where the last valid information was received
    draw_screen = true;                              // indicates that the screen needs to be redrawn because there are GPS updates
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

void update_gps_trip() {                                        // Updates trip statistics using the current valid GPS/GNSS position
  if (gps_data.latitude != 0.0 && gps_data.longitude != 0.0) {  // If there is latitude and longitude
    if (gps_trip.trip_start_millis == 0) {                      // If the start of the trip has not been defined
      gps_trip.trip_start_millis = millis();                    // Stores the trip start time
    }
    if (gps_trip.start_latitude == 0.0 && gps_trip.start_longitude == 0.0) {  // If there is no starting point for the trip
      gps_trip.start_latitude = gps_data.latitude;                            // Stores the initial latitude
      gps_trip.start_longitude = gps_data.longitude;                          // Stores the initial longitude
    } else {
      // Measures the distance between the trip's starting point and the current location
      gps_trip.distance_from_start = TinyGPSPlus::distanceBetween(gps_trip.start_latitude, gps_trip.start_longitude, gps_data.latitude, gps_data.longitude);
    }
  }
  if (gps_data.altitude > gps_trip.max_altitude) {  // If the current altitude is greater than the maximum recorded altitude
    gps_trip.max_altitude = gps_data.altitude;      // A new maximum altitude is recorded
  }
  if (gps_data.altitude < gps_trip.min_altitude || gps_trip.min_altitude == 0.0) {  // If the current altitude is lower than the minimum recorded altitude, or if the altitude is 0.
    gps_trip.min_altitude = gps_data.altitude;                                      // Records a new minimum altitude
  }
  if (gps_data.speed > gps_trip.max_speed) {  // If the current speed is greater than the maximum recorded speed
    gps_trip.max_speed = gps_data.speed;      // Now, the maximum speed is the current speed
  }
  if (gps_data.speed < gps_trip.min_speed || gps_trip.min_speed == 0.0) {  // If the current speed is less than the minimum recorded speed, or if the speed is 0
    gps_trip.min_speed = gps_data.speed;                                   // the minimum speed is equal to the current speed
  }
}

void update_gps_route() {  // Accumulates route distance and moving time using consecutive GPS/GNSS positions
  if (gps_data.latitude == 0.0 && gps_data.longitude == 0.0) {
    return;
  }
  if (gps_data.speed < settings.min_moving_speed) {  // Ignore position changes while speed is low, to prevent GPS position fluctuation from increasing the route distance
    gps_trip.route_last_latitude = 0.0;
    gps_trip.route_last_longitude = 0.0;
    gps_trip.last_moving_millis = 0;
    return;
  }
  unsigned long current_millis = millis();  // Stores the current value of millis()
  if (gps_trip.route_last_latitude == 0.0 && gps_trip.route_last_longitude == 0.0) {
    gps_trip.route_last_latitude = gps_data.latitude;
    gps_trip.route_last_longitude = gps_data.longitude;
    gps_trip.last_moving_millis = current_millis;
    return;
  }
  // Calculates the segment distance, which is the difference between the location at the time of the last update and the current location
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

void update_time_in_motion() {                                                                 // Updates the total time spent in motion, excluding periods when the device is stopped
  unsigned long current_millis = millis();                                                     // Gets the current system time in milliseconds since the device started
  unsigned long elapsed_millis = current_millis - gps_trip.last_time_in_motion_update_millis;  // Calculates the time elapsed since the last time-in-motion counter update
  gps_trip.last_time_in_motion_update_millis = current_millis;                                 // Stores the current timestamp to calculate the elapsed time during the next update
  bool speed_data_is_fresh = current_millis - gps_trip.last_speed_update_millis < 2000;        // Checks whether the last speed update was received less than 2 seconds ago. This prevents outdated speed data from keeping the motion counter running
  if (speed_data_is_fresh && gps_trip.is_moving) {                                             // Accumulates the elapsed time only if the speed data is recent and the device is moving. The current speed must be equal to or greater than the minimum speed
    gps_trip.time_in_motion_millis += elapsed_millis;                                          // Adds the elapsed milliseconds to the total time spent in motion
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
    case 2:  // If the screen_i index is 2, it goes to the speed screen
      screen_speed();
      break;
    case 3:  // If the screen_i index is 3, it goes to the altitude screen
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
  if (draw_screen) {
    // Check if the timeout warning screen should not be displayed and if the timeout is longer than acceptable
    if (settings.gps_timeout_seconds != 0 && system_state.display_timeout_warning == false && millis() - gps_time.last_update_millis >= (settings.gps_timeout_seconds * 1000)) {
      // Draw a warning symbol
      M5.Display.fillTriangle(M5.Display.width() - 5, M5.Display.height() - 5, M5.Display.width() - 26, M5.Display.height() - 5, M5.Display.width() - 15, M5.Display.height() - 26, YELLOW);
      M5.Display.setTextDatum(middle_center);
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(settings.background_color);
      M5.Display.drawString("!", M5.Display.width() - 14, M5.Display.height() - 12);
      // Shows a warning icon when GNSS communication has timed out but the warning screen is not active
    }
  }
  if (draw_screen || toggle_screen) {                   // Checks if the screen has been updated
    system_state.last_screen_update_millis = millis();  // Update system_state.last_screen_update_millis with the current millis() value
  }
  draw_screen = false;    // draw_screen is set to false, as the screen has already been drawn
  toggle_screen = false;  // toggle_screen is set to false because the screen has already been toggled
}

void screen_general() {
  if (M5.BtnA.wasPressed()) {                 // If button A is pressed
    screen_general_i = screen_general_i + 1;  // The General screen index increases
    if (screen_general_i > 3) {               // If the index of the General screen is greater than 3 (3 is the limit)
      screen_general_i = 0;                   // The index returns to 0 (goes back to the beginning)
    }
    draw_screen = true;    // Set draw_screen to true to indicate that the screen needs to be redrawn
    toggle_screen = true;  // Set toggle_screen to true to indicate that the screen has been toggled
  }

  if (draw_screen) {              // Whether the screen should be drawn
    if (screen_general_i == 0) {  // If the General screen index is equal to 0
      // Only index 0 of the General screen has a horizontal orientation
      M5.Display.setRotation(horizontal_screen_rotation);  // Set the screen rotation to landscape orientation, with button A (blue button) facing either left or right
    } else {                                               // If the General screen index is different from 0
      M5.Display.setRotation(vertical_screen_rotation);    // Set the screen rotation to portrait orientation, with button A (the blue button) facing either down or up
    }
  }

  switch (screen_general_i) {
    case 0:                // If the General screen index is equal to 0
      screen_general_1();  // it goes to subscreen 1 (the first General subscreen)
      break;
    case 1:                // If the General screen index is equal to 1
      screen_general_2();  // it goes to subscreen 2 (the second General subscreen)
      break;
    case 2:
      screen_general_3();
      break;
    case 3:
      screen_general_4();
      break;
    default:                 // If the General screen index is not among the Switch options
      screen_general_1();    // Go to the General 1 subscreen
      screen_general_i = 0;  // Sets the index for the General screen to 0 (returns to the beginning)
      break;
  }

  if (screen_general_i != 0) {  // If the General screen index is different from 0
    // It will set the Text Datum, font, and text color to the default
    M5.Display.setTextDatum(top_left);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextColor(settings.theme_color);
  }
}

void screen_general_1() {
  if (draw_screen) {
    screen_header("General");  // Calls the header function, which will draw the header with the screen name and the battery percentage
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
    int cursor_x_speed_unit = M5.Display.textWidth("SPEED: ");  // Measure the width of the text
    M5.Display.setTextSize(2);
    M5.Display.print(convert_speed(gps_data.speed, false));
    cursor_x_speed_unit = cursor_x_speed_unit + M5.Display.textWidth(convert_speed(gps_data.speed, false));
    M5.Display.setTextSize(1);
    M5.Display.setCursor(13 + cursor_x_speed_unit, 72);
    M5.Display.println(settings.units_speed_measurement);
    M5.Display.setCursor(130, 65);
    M5.Display.setTextSize(1);
    M5.Display.print("ALT: ");
    int cursor_x_altitude_unit = M5.Display.textWidth("ALT: ");
    M5.Display.setTextSize(2);
    if (settings.unit_altitude_measurement == "Meters") {
      M5.Display.print(gps_data.altitude, 0);
      cursor_x_altitude_unit = cursor_x_altitude_unit + M5.Display.textWidth(String(gps_data.altitude, 0));
    } else {
      M5.Display.print(gps_data.altitude * 3.28084, 0);
      cursor_x_altitude_unit = cursor_x_altitude_unit + M5.Display.textWidth(String(gps_data.altitude * 3.28084, 0));
    }
    M5.Display.setTextSize(1);
    M5.Display.setCursor(133 + cursor_x_altitude_unit, 72);
    if (settings.unit_altitude_measurement == "Meters") {
      M5.Display.println("m");
    } else {
      M5.Display.println("ft");
    }
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
    int latitude_y = 95;
    int longitude_y = 115;
    M5.Display.setCursor(10, latitude_y);
    M5.Display.setTextSize(1);
    M5.Display.print("LAT: ");
    M5.Display.setTextSize(2);
    int latitude_start_x = M5.Display.getCursorX();
    M5.Display.println(latitude_str);
    M5.Display.setCursor(10, longitude_y);
    M5.Display.setTextSize(1);
    M5.Display.print("LNG: ");
    M5.Display.setTextSize(2);
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
  }
}

void screen_general_2() {
  if (draw_screen) {
    screen_header_vertical("General", true);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Speed - " + settings.units_speed_measurement, M5.Display.width() / 2, 25);
    M5.Display.setTextSize(4);
    M5.Display.drawString(convert_speed(gps_data.speed, false), M5.Display.width() / 2, 40);
    M5.Display.setTextSize(1);
    if (settings.unit_altitude_measurement == "Meters") {
      M5.Display.drawString("Altitude - Meters", M5.Display.width() / 2, 85);
      M5.Display.setTextSize(4);
      M5.Display.drawString(String((int)gps_data.altitude), M5.Display.width() / 2, 100);
    } else {
      M5.Display.drawString("Altitude - Feet", M5.Display.width() / 2, 85);
      M5.Display.setTextSize(4);
      M5.Display.drawString(String((int)(gps_data.altitude * 3.28084)), M5.Display.width() / 2, 100);
    }
    M5.Display.setTextSize(1);
    M5.Display.drawString("SAT", M5.Display.width() / 4, 145);
    M5.Display.drawString("HDOP", (M5.Display.width() / 4) * 3, 145);
    M5.Display.setTextSize(3);
    M5.Display.drawString(String(gps_data.satellites), M5.Display.width() / 4, 160);
    if (gps_data.hdop < 10) {
      M5.Display.drawString(String(gps_data.hdop, 1), (M5.Display.width() / 4) * 3, 160);
    } else {
      M5.Display.drawString(String((int)gps_data.hdop), (M5.Display.width() / 4) * 3, 160);
    }
    String direction_symbol;
    if (gps_data.course >= 337.5 || gps_data.course < 22.5) {
      direction_symbol = "N";
    } else if (gps_data.course < 67.5) {
      direction_symbol = "NE";
    } else if (gps_data.course < 112.5) {
      direction_symbol = "E";
    } else if (gps_data.course < 157.5) {
      direction_symbol = "SE";
    } else if (gps_data.course < 202.5) {
      direction_symbol = "S";
    } else if (gps_data.course < 247.5) {
      direction_symbol = "SW";
    } else if (gps_data.course < 292.5) {
      direction_symbol = "W";
    } else {
      direction_symbol = "NW";
    }
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
    int latitude_y = 210;
    int longitude_y = 225;
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(top_left);
    M5.Display.drawString("Course:", 15, 195);
    M5.Display.setTextDatum(top_right);
    M5.Display.drawString(String((int)gps_data.course), M5.Display.width() - 40, 195);
    M5.Display.drawCircle(M5.Display.width() - 37, 197, 1, settings.theme_color);
    M5.Display.drawString(direction_symbol, M5.Display.width() - 15, 195);
    M5.Display.setTextDatum(top_left);
    M5.Display.drawString("LAT:", 15, latitude_y);
    M5.Display.setTextDatum(top_right);
    M5.Display.drawString(latitude_str, M5.Display.width() - 15, latitude_y);
    M5.Display.setTextDatum(top_left);
    M5.Display.drawString("LNG:", 15, longitude_y);
    M5.Display.setTextDatum(top_right);
    M5.Display.drawString(longitude_str, M5.Display.width() - 15, longitude_y);
    int latitude_start_x = M5.Display.width() - 15 - M5.Display.textWidth(latitude_str);
    int longitude_start_x = M5.Display.width() - 15 - M5.Display.textWidth(longitude_str);
    if (draw_degree_symbol) {
      // Latitude
      int latitude_degrees = abs((int)gps_data.latitude);
      int latitude_degree_width = M5.Display.textWidth(String(latitude_degrees));
      latitude_degree_x = latitude_start_x + latitude_degree_width + 2;
      M5.Display.drawCircle(latitude_degree_x, latitude_y + 2, 1, settings.theme_color);
      // Longitude
      int longitude_degrees = abs((int)gps_data.longitude);
      int longitude_degree_width = M5.Display.textWidth(String(longitude_degrees));
      longitude_degree_x = longitude_start_x + longitude_degree_width + 2;
      M5.Display.drawCircle(longitude_degree_x, longitude_y + 2, 1, settings.theme_color);
    }
  }
}

void screen_general_3() {
  if (draw_screen) {
    screen_header_vertical("General", true);
    char time_str[12];
    if (settings.time_format == "24-hour") {
      sprintf(time_str, "%02d:%02d:%02d", gps_time.hour, gps_time.minute, gps_time.second);  // Formats the time as HH:MM:SS
    } else {
      int hour = gps_time.hour;
      hour = hour % 12;
      if (hour == 0) {
        hour = 12;
      }
      sprintf(time_str, "%02d:%02d", hour, gps_time.minute);  // Formats the time using the 12-hour format
    }
    char date_str[11];
    if (settings.date_format == "YYYY-MM-DD") {
      sprintf(date_str, "%04d-%02d-%02d", gps_time.year, gps_time.month, gps_time.day);
    } else if (settings.date_format == "MM/DD/YYYY") {
      sprintf(date_str, "%02d/%02d/%04d", gps_time.month, gps_time.day, gps_time.year);
    } else {
      sprintf(date_str, "%02d/%02d/%04d", gps_time.day, gps_time.month, gps_time.year);
    }
    if (settings.time_format == "24-hour") {
      M5.Display.setTextSize(0.5f);
      M5.Display.setFont(&fonts::Font7);
      M5.Display.drawString(time_str, M5.Display.width() / 2, 25);
    } else {
      M5.Display.setFont(&fonts::Font0);
      M5.Display.setTextSize(3);
      int time_indicators_width = M5.Display.textWidth("AM");
      M5.Display.setFont(&fonts::Font7);
      M5.Display.setTextSize(0.5f);
      int time_text_width = M5.Display.textWidth(time_str);
      M5.Display.drawString(time_str, (M5.Display.width() / 2) - (time_indicators_width / 2), 25);
      M5.Display.setFont(&fonts::Font0);
      M5.Display.setTextSize(3);
      if (gps_time.hour >= 12) {
        M5.Display.drawString("PM", (M5.Display.width() / 2) + (time_text_width / 2) + 5, 25);
      } else {
        M5.Display.drawString("AM", (M5.Display.width() / 2) + (time_text_width / 2) + 5, 25);
      }
    }
    M5.Display.setTextSize(0.8f);
    M5.Display.setFont(&fonts::FreeSans12pt7b);
    M5.Display.drawString(date_str, (M5.Display.width() / 2), 60);
    M5.Display.drawLine(0, 85, M5.Display.width(), 85, settings.theme_color);
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
    int latitude_y = 110;
    int longitude_y = 150;
    int latitude_start_x = 10;
    int longitude_start_x = 10;
    M5.Display.setTextDatum(top_left);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString("Latitude:", 10, 95);
    M5.Display.setTextSize(0.6f);
    M5.Display.setFont(&fonts::FreeSans18pt7b);
    M5.Display.drawString(latitude_str, latitude_start_x, latitude_y);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString("Longitude:", 10, 135);
    M5.Display.setTextSize(0.6f);
    M5.Display.setFont(&fonts::FreeSans18pt7b);
    M5.Display.drawString(longitude_str, longitude_start_x, longitude_y);
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
    M5.Display.drawLine(0, 180, M5.Display.width(), 180, settings.theme_color);
    M5.Display.drawLine(M5.Display.width() / 2, 180, M5.Display.width() / 2, M5.Display.height(), settings.theme_color);
    M5.Display.setTextDatum(top_center);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextSize(1);
    M5.Display.drawString("SAT", M5.Display.width() / 4, 190);
    M5.Display.drawString("HDOP", (M5.Display.width() / 4) * 3, 190);
    M5.Display.setTextSize(0.7f);
    M5.Display.setFont(&fonts::FreeSans18pt7b);
    M5.Display.drawString(String(gps_data.satellites), M5.Display.width() / 4, 205);
    if (gps_data.hdop < 10) {
      M5.Display.drawString(String(gps_data.hdop, 1), (M5.Display.width() / 4) * 3, 205);
    } else {
      M5.Display.drawString(String((int)gps_data.hdop), (M5.Display.width() / 4) * 3, 205);
    }
    M5.Display.setFont(&fonts::Font0);
  }
}

void screen_general_4() {
  if (draw_screen) {
    screen_header_vertical("General", false);
    M5.Display.fillRoundRect(10, 25, M5.Display.width() - 20, 62, 10, settings.background_color);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Speed - " + settings.units_speed_measurement, M5.Display.width() / 2, 32);
    M5.Display.setFont(&fonts::FreeSans18pt7b);
    M5.Display.drawString(convert_speed(gps_data.speed, false), M5.Display.width() / 2, 50);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.fillRoundRect(10, 97, M5.Display.width() - 20, 62, 10, settings.background_color);
    if (settings.unit_altitude_measurement == "Meters") {
      M5.Display.drawString("Altitude - Meters", M5.Display.width() / 2, 106);
      M5.Display.setFont(&fonts::FreeSans18pt7b);
      M5.Display.drawString(String((int)gps_data.altitude), M5.Display.width() / 2, 123);
      M5.Display.setFont(&fonts::Font0);
    } else {
      M5.Display.drawString("Altitude - Feet", M5.Display.width() / 2, 106);
      M5.Display.setFont(&fonts::FreeSans18pt7b);
      M5.Display.drawString(String((int)(gps_data.altitude * 3.28084)), M5.Display.width() / 2, 123);
      M5.Display.setFont(&fonts::Font0);
    }
    String direction;
    if (gps_data.course >= 337.5 || gps_data.course < 22.5) {
      direction = "North";
    } else if (gps_data.course < 67.5) {
      direction = "Northeast";
    } else if (gps_data.course < 112.5) {
      direction = "East";
    } else if (gps_data.course < 157.5) {
      direction = "Southeast";
    } else if (gps_data.course < 202.5) {
      direction = "South";
    } else if (gps_data.course < 247.5) {
      direction = "Southwest";
    } else if (gps_data.course < 292.5) {
      direction = "West";
    } else {
      direction = "Northwest";
    }
    M5.Display.fillRoundRect(10, 168, M5.Display.width() - 20, 62, 10, settings.background_color);
    M5.Display.drawString("Course: " + direction, M5.Display.width() / 2, 175);
    M5.Display.setFont(&fonts::FreeSans18pt7b);
    M5.Display.drawString(String((int)gps_data.course), M5.Display.width() / 2, 192);
    int course_text_width = (M5.Display.textWidth(String((int)gps_data.course)) / 2) + (M5.Display.width() / 2);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawCircle(course_text_width + 7, 198, 4, settings.theme_color);
    M5.Display.drawCircle(course_text_width + 7, 198, 5, settings.theme_color);
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
        distance_str = String(gps_trip.distance_from_start, 0) + " m";
      } else {
        if (gps_trip.distance_from_start > 10000) {
          distance_str = String(gps_trip.distance_from_start / 1000.0, 0) + " km";
        } else {
          distance_str = String(gps_trip.distance_from_start / 1000.0, 1) + " km";
        }
      }
    } else {
      double feet = gps_trip.distance_from_start * 3.28084;
      if (feet < 5280.0) {
        distance_str = String(feet, 0) + " ft";
      } else {
        double miles = gps_trip.distance_from_start * 0.000621371;
        if (miles > 1000) {
          distance_str = String(miles, 0) + " mi";
        } else {
          distance_str = String(miles, 1) + " mi";
        }
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
  if (M5.BtnA.wasPressed()) {
    screen_route_i = screen_route_i + 1;
    if (screen_route_i > 2) {
      screen_route_i = 0;
    }
    draw_screen = true;    // Set draw_screen to true to indicate that the screen needs to be redrawn
    toggle_screen = true;  // Set toggle_screen to true to indicate that the screen has been toggled
  }

  if (draw_screen) {
    if (screen_route_i == 0) {
      M5.Display.setRotation(horizontal_screen_rotation);  // Set the screen rotation to landscape orientation, with button A (blue button) facing either left or right
    } else {
      M5.Display.setRotation(vertical_screen_rotation);  // Set the screen rotation to portrait orientation, with button A (the blue button) facing either down or up
    }
  }

  switch (screen_route_i) {
    case 0:
      screen_route_stats_1();
      break;
    case 1:
      screen_route_stats_2();
      break;
    case 2:
      screen_route_stats_3();
      break;
    default:
      screen_route_stats_1();
      screen_route_i = 0;
      break;
  }

  if (screen_route_i != 0) {
    M5.Display.setTextDatum(top_left);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextColor(settings.theme_color);
  }
}

void screen_route_stats_1() {
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

void screen_route_stats_2() {
  if (draw_screen || toggle_screen) {
    screen_header_vertical("Route Stats", true);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString("CURRENT SPEED - " + settings.units_speed_measurement, M5.Display.width() / 2, 25);
    if (gps_data.speed > 999) {
      M5.Display.setTextSize(1.1f);
    } else {
      M5.Display.setTextSize(1.3f);
    }
    M5.Display.setFont(&fonts::Font7);
    M5.Display.drawString(convert_speed(gps_data.speed, false), M5.Display.width() / 2, 40);
    M5.Display.drawLine(0, 110, M5.Display.width(), 110, settings.theme_color);
    M5.Display.drawLine(M5.Display.width() / 2, 110, M5.Display.width() / 2, 175, settings.theme_color);
    M5.Display.drawLine(0, 125, M5.Display.width(), 125, settings.theme_color);
    int min_speed_bar = 0;
    int middle_speed_bar = M5.Display.width() / 2;
    int max_speed_bar = M5.Display.width();
    int start_y_bar = 110;
    int end_y_bar = 125;
    float average_speed = gps_trip.average_moving_speed;
    float current_speed = gps_data.speed;
    float bar_size = 0;
    if (gps_data.speed >= settings.min_moving_speed) {
      if (current_speed < average_speed && average_speed > gps_trip.min_speed) {
        bar_size = (average_speed - current_speed) / (average_speed - gps_trip.min_speed) * middle_speed_bar;  // Calculate the bar size proportionally between minimum and average speed
        bar_size = constrain(bar_size, 0, middle_speed_bar);                                                   // Keep the bar within the available screen area
        if (bar_size > 0) {
          M5.Display.fillRect(middle_speed_bar - (int)bar_size, start_y_bar, (int)bar_size, end_y_bar - start_y_bar, settings.theme_color);  // Draw the bar to the left of the reference line
        }
      } else if (current_speed > average_speed) {
        float max_range = gps_trip.max_speed - average_speed;  // Calculate the bar size proportionally to the speed difference
        if (max_range > 0) {
          bar_size = (current_speed - average_speed) / max_range * (max_speed_bar - middle_speed_bar);
          bar_size = constrain(bar_size, 0, max_speed_bar - middle_speed_bar);  // Keep the bar within the available screen area
          if (bar_size > 0) {
            M5.Display.fillRect(middle_speed_bar, start_y_bar, (int)bar_size, end_y_bar - start_y_bar, settings.theme_color);  // Draw the bar to the right of the reference line
          }
        }
      }
    }
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextSize(1);
    M5.Display.drawString("SPEED MIN", M5.Display.width() / 4, 135);
    M5.Display.drawString("SPEED MAX", ((M5.Display.width() / 4) * 3) + 3, 135);
    M5.Display.setTextSize(2);
    M5.Display.drawString(convert_speed(gps_trip.min_speed, false), M5.Display.width() / 4, 150);
    M5.Display.drawString(convert_speed(gps_trip.max_speed, false), ((M5.Display.width() / 4) * 3) + 3, 150);
    M5.Display.drawLine(0, 175, M5.Display.width(), 175, settings.theme_color);
    M5.Display.setTextSize(1);
    M5.Display.drawString("AVERAGE MOVING SPEED", M5.Display.width() / 2, 185);
    M5.Display.setTextSize(1);
    int average_speed_unit_width = M5.Display.textWidth(settings.units_speed_measurement);
    M5.Display.setTextSize(4);
    M5.Display.drawString(convert_speed(gps_trip.average_moving_speed, false), (M5.Display.width() / 2) - (average_speed_unit_width / 2), 200);
    int average_speed_text_width = M5.Display.textWidth(convert_speed(gps_trip.average_moving_speed, false));
    M5.Display.setTextSize(1);
    M5.Display.drawString(settings.units_speed_measurement, (M5.Display.width() / 2) + (average_speed_text_width / 2) + 3, 221);
  }
}

void screen_route_stats_3() {
  if (draw_screen || toggle_screen) {
    screen_header_vertical("Route Stats", true);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString("ROUTE DISTANCE", M5.Display.width() / 2, 25);
    String distance_str;
    String distance_unit;
    if (settings.unit_distance_measurement == "Metric") {
      if (gps_trip.route_distance < 1000.0) {
        distance_str = String(gps_trip.route_distance, 0);
        distance_unit = "m";
      } else {
        if (gps_trip.route_distance < 10000.0) {
          distance_str = String(gps_trip.route_distance / 1000.0, 1);
        } else {
          distance_str = String(gps_trip.route_distance / 1000.0, 0);
        }
        distance_unit = "km";
      }
    } else {
      double feet = gps_trip.route_distance * 3.28084;
      if (feet < 5280.0) {
        distance_str = String(feet, 0);
        distance_unit = "ft";
      } else {
        double miles = gps_trip.route_distance * 0.000621371;
        if (miles < 10.0) {
          distance_str = String(miles, 1);
        } else {
          distance_str = String(miles, 0);
        }
        distance_unit = "mi";
      }
    }
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextSize(2);
    int distance_unit_width = M5.Display.textWidth(distance_unit);
    M5.Display.setTextSize(0.8f);
    M5.Display.setFont(&fonts::Font7);
    M5.Display.drawString(distance_str, (M5.Display.width() / 2) - (distance_unit_width / 2), 40);
    int distance_text_width = M5.Display.textWidth(distance_str);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextSize(2);
    M5.Display.drawString(distance_unit, (M5.Display.width() / 2) + (distance_text_width / 2) + 3, 62);
    M5.Display.drawLine(0, 90, M5.Display.width(), 90, settings.theme_color);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString("STRAIGHT-LINE DISTANCE", M5.Display.width() / 2, 100);
    if (settings.unit_distance_measurement == "Metric") {
      if (gps_trip.distance_from_start < 1000.0) {
        distance_str = String((int)gps_trip.distance_from_start);
        distance_unit = "m";
      } else {
        if (gps_trip.distance_from_start < 10000.0) {
          distance_str = String(gps_trip.distance_from_start / 1000.0, 1);
        } else {
          distance_str = String(gps_trip.distance_from_start / 1000.0, 0);
        }
        distance_unit = "km";
      }
    } else {
      double feet = gps_trip.distance_from_start * 3.28084;
      if (feet < 5280.0) {
        distance_str = String((int)feet);
        distance_unit = "ft";
      } else {
        double miles = gps_trip.distance_from_start * 0.000621371;
        if (miles < 10.0) {
          distance_str = String(miles, 1);
        } else {
          distance_str = String(miles, 0);
        }
        distance_unit = "mi";
      }
    }
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextSize(2);
    distance_unit_width = M5.Display.textWidth(distance_unit);
    M5.Display.setTextSize(0.8f);
    M5.Display.setFont(&fonts::Font7);
    M5.Display.drawString(distance_str, (M5.Display.width() / 2) - (distance_unit_width / 2), 115);
    distance_text_width = M5.Display.textWidth(distance_str);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.setTextSize(2);
    M5.Display.drawString(distance_unit, (M5.Display.width() / 2) + (distance_text_width / 2) + 3, 137);
    M5.Display.drawLine(0, 165, M5.Display.width(), 165, settings.theme_color);
    M5.Display.setTextSize(1);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString("TIME IN MOTION", M5.Display.width() / 2, 175);
    String elapsed_time = format_millis_in_motion_clock(gps_trip.time_in_motion_millis, false, false);
    if (elapsed_time.length() < 6) {
      M5.Display.setTextSize(0.8f);
    } else {
      M5.Display.setTextSize(0.5f);
    }
    M5.Display.setFont(&fonts::Font7);
    M5.Display.drawString(elapsed_time, M5.Display.width() / 2, 190);
  }
}

void screen_course() {
  if (toggle_screen) {
    M5.Display.fillScreen(settings.background_color);
    M5.Display.fillRect(0, 0, 173, 25, settings.theme_color);
    M5.Display.fillRect(0, 110, 173, 25, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    M5.Display.setTextDatum(top_left);
    M5.Display.drawString("Course", 10, 5);
    M5.Display.fillCircle(172, 67, 73, settings.background_color);
    M5.Display.fillCircle(172, 67, 67, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
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
    if (data_comparison.line_course_x != endX || data_comparison.line_course_y != endY || toggle_screen || (gps_data.speed < 1 && last_speed_course >= 1)) {
      if (data_comparison.line_course_x != 0 && data_comparison.line_course_y != 0) {
        M5.Display.drawWideLine(centerX, centerY, data_comparison.line_course_x, data_comparison.line_course_y, 5, settings.theme_color);
      }
      M5.Display.drawWideLine(centerX, centerY, endX, endY, 3, settings.background_color);  // Draw a thick line from the center to the tip of the pointer, using the background color
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
      M5.Display.fillCircle(172, 67, 7, settings.background_color);
      M5.Display.fillCircle(172, 67, 2, settings.theme_color);
      M5.Display.fillRect(0, 25, 103, 85, settings.background_color);
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
      last_speed_course = gps_data.speed;
      M5.Display.fillRect(0, 110, 103, 25, settings.theme_color);
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(settings.background_color);
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
  }  // 18.5
  if (draw_screen || toggle_screen) {
    screen_header("Session Info");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_left);
    M5.Display.drawString("Time in Motion: ", 10, 43);
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_right);
    M5.Display.drawString(format_millis_in_motion_clock(gps_trip.time_in_motion_millis, false, true), M5.Display.width() - 10, 43);

    M5.Display.drawLine(0, 62, M5.Display.width(), 62, settings.theme_color);

    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_left);
    M5.Display.drawString("Trip Time: ", 10, 81);
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_right);
    if (gps_trip.trip_start_millis == 0) {
      M5.Display.drawString("No Fix", M5.Display.width() - 10, 81);
    } else {
      M5.Display.drawString(format_millis_in_motion_clock(gps_trip.trip_start_millis, true, true), M5.Display.width() - 10, 81);
    }

    M5.Display.drawLine(0, 100, M5.Display.width(), 100, settings.theme_color);

    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_left);
    M5.Display.drawString("Fix Time: ", 10, 119);
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_right);
    if (gps_time.first_fix_millis == 0) {
      M5.Display.drawString("No Fix", M5.Display.width() - 10, 119);
    } else {
      M5.Display.drawString(format_millis_in_motion_clock(gps_time.first_fix_millis, true, true), M5.Display.width() - 10, 119);
    }
  }
}

void screen_about() {
  if (M5.BtnA.wasReleased()) {
    if (screen_about_i) {
      screen_about_i = false;
    } else {
      screen_about_i = true;
    }
    toggle_screen = true;
  }
  if (screen_about_i) {
    if (toggle_screen) {
      screen_header("About");
      M5.Display.setTextSize(1);
      M5.Display.setTextDatum(top_center);
      M5.Display.drawString("Developed by", M5.Display.width() / 2, 30);
      M5.Display.setTextSize(2);
      M5.Display.drawString("ViniciusHNF", M5.Display.width() / 2, 45);
      M5.Display.setTextSize(1);
      M5.Display.drawString("For more information or questions,", M5.Display.width() / 2, 70);
      M5.Display.drawString("visit the project page on GitHub:", M5.Display.width() / 2, 83);
      M5.Display.fillRoundRect(10, 95, 220, 33, 10, settings.theme_color);
      M5.Display.setTextColor(settings.background_color);
      M5.Display.setTextDatum(top_left);
      M5.Display.drawString("https://github.com/viniciushnf/", 25, 100);
      M5.Display.drawString("M5StickS3-GPS-GNSS-Monitor", 25, 115);
    }
  } else {
    if (toggle_screen) {
      screen_header("About");
      M5.Display.setTextSize(1);
      M5.Display.setTextDatum(top_center);
      M5.Display.drawString("GPS & GNSS Monitor - V " + String(VERSION_FIRMWARE, 1), M5.Display.width() / 2, 30);
      M5.Display.setTextDatum(top_left);
      M5.Display.setCursor(10, 45);
      M5.Display.print("GPIO RX: " + String(settings.gpio_rx));
      M5.Display.print(" - GPIO TX: " + String(settings.gpio_tx));
      M5.Display.setCursor(10, 60);
      M5.Display.print("Baud rate: " + String(settings.module_baudrate));
      M5.Display.drawLine(0, 75, M5.Display.width(), 75, settings.theme_color);
      M5.Display.setTextDatum(top_center);
      M5.Display.setTextSize(1.4f);
      M5.Display.drawString("Developed by ViniciusHNF", M5.Display.width() / 2, 85);
      M5.Display.setTextDatum(top_left);
      M5.Display.setTextSize(1);
      M5.Display.setCursor(10, 105);
      M5.Display.print("> github.com/viniciushnf");
      M5.Display.setCursor(10, 120);
      M5.Display.print("> youtube.com/@ViniciusHNF");
    }
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
          screen_settings_i = 19;                                       // screen_settings_i receives the value 19, which is the index of the last screen
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
        screen_settings_theme_color();
        break;
      case 1:
        screen_settings_background_color();
        break;
      case 2:
        screen_settings_brightness();
        break;
      case 3:
        screen_settings_screen_timeout();
        break;
      case 4:
        screen_settings_flip_screen_horizontally();
        break;
      case 5:
        screen_settings_flip_screen_vertically();
        break;
      case 6:
        screen_settings_screen_rotate();
        break;
      case 7:
        screen_settings_coord_format();
        break;
      case 8:
        screen_settings_unit_speed();
        break;
      case 9:
        screen_settings_min_trip_speed();
        break;
      case 10:
        screen_settings_unit_altitude();
        break;
      case 11:
        screen_settings_unit_distance();
        break;
      case 12:
        screen_settings_timezone();
        break;
      case 13:
        screen_settings_date_format();
        break;
      case 14:
        screen_settings_time_format();
        break;
      case 15:
        screen_settings_screen_update_interval();
        break;
      case 16:
        screen_settings_module_timeout();
        break;
      case 17:
        screen_settings_basic_settings();
        break;
      case 18:
        screen_settings_reset_trip();
        break;
      case 19:
        screen_settings_exit();
        break;
      default:
        screen_settings_theme_color();
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

void screen_settings_theme_color() {
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
    if (settings.background_color == theme_color_options[selected_color_i]) {  // Checks whether the background color is the same as the theme color
      selected_color_i = selected_color_i + 1;                                 // Move on to the next color. The theme color cannot be the same as the background color, otherwise the screen would end up being just a single color
    }
    if (selected_color_i > (sizeof(theme_color_options) / sizeof(theme_color_options[0])) - 1) {  // Check again whether the index is equal to or greater than the array
      selected_color_i = 0;                                                                       // selected_color_i returns to 0
    }
    settings.theme_color = theme_color_options[selected_color_i];  // The theme_color will be the selected_color_i index of the theme_color_options array
    draw_screen = true;
    toggle_screen_settings = true;  // toggle_screen_settings is set to true to force the header to update with the new color
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Theme Color");
    show_defined_configuration(theme_color_options_string[selected_color_i]);
    settings_footer("theme color");
  }
}

void screen_settings_background_color() {
  byte selected_color_i = 0;  // Finding the already defined background color
  for (int i_color = 0; i_color < sizeof(theme_color_options) / sizeof(theme_color_options[0]); i_color++) {
    if (theme_color_options[i_color] == settings.background_color) {
      selected_color_i = i_color;
    }
  }
  if (M5.BtnA.wasPressed()) {
    selected_color_i = selected_color_i + 1;                                                      // 1 is added to the value of selected_color_i
    if (selected_color_i > (sizeof(theme_color_options) / sizeof(theme_color_options[0])) - 1) {  // If the value of selected_color_i is equal to the number of indices in theme_color_options
      selected_color_i = 0;                                                                       // selected_color_i returns to 0
    }
    if (settings.theme_color == theme_color_options[selected_color_i]) {  // Checks whether the background color is the same as the theme color
      selected_color_i = selected_color_i + 1;                            // Move on to the next color. The theme color cannot be the same as the background color, otherwise the screen would end up being just a single color
    }
    if (selected_color_i > (sizeof(theme_color_options) / sizeof(theme_color_options[0])) - 1) {  // Check again whether the index is equal to or greater than the array
      selected_color_i = 0;                                                                       // selected_color_i returns to 0
    }
    settings.background_color = theme_color_options[selected_color_i];  // The theme_color will be the selected_color_i index of the theme_color_options array
    draw_screen = true;
    toggle_screen_settings = true;  // toggle_screen_settings is set to true to force the header to update with the new color
    save_settings();
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Background Color");
    show_defined_configuration(theme_color_options_string[selected_color_i]);
    settings_footer("background color");
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
    settings_footer("screen brightness");
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
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Time before the screen turns", M5.Display.width() / 2, 35);
    M5.Display.drawString("off after no button interaction", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    M5.Display.drawString(str_screen_timeout, M5.Display.width() / 2, 77);
    settings_footer("screen timeout");
  }
}

void screen_settings_flip_screen_horizontally() {
  if (M5.BtnA.wasPressed()) {
    if (settings.flip_screen_horizontally) {
      settings.flip_screen_horizontally = false;
    } else {
      settings.flip_screen_horizontally = true;
    }
    save_settings();
    apply_rotation();
    draw_screen = true;
    toggle_screen_settings = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Screen Flip H");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Rotates horizontal screens", M5.Display.width() / 2, 35);
    M5.Display.drawString("by 180 degrees when enabled", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    if (settings.flip_screen_horizontally) {
      M5.Display.drawString("YES", M5.Display.width() / 2, 77);
    } else {
      M5.Display.drawString("NO", M5.Display.width() / 2, 77);
    }
    settings_footer("screen flip H");
  }
}

void screen_settings_flip_screen_vertically() {
  if (M5.BtnA.wasPressed()) {
    if (settings.flip_screen_vertically) {
      settings.flip_screen_vertically = false;
    } else {
      settings.flip_screen_vertically = true;
    }
    save_settings();
    apply_rotation();
    draw_screen = true;
    toggle_screen_settings = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Screen Flip V");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Rotates vertical screens", M5.Display.width() / 2, 35);
    M5.Display.drawString("by 180 degrees when enabled", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    if (settings.flip_screen_vertically) {
      M5.Display.drawString("YES", M5.Display.width() / 2, 77);
    } else {
      M5.Display.drawString("NO", M5.Display.width() / 2, 77);
    }
    settings_footer("screen flip V");
  }
}

void screen_settings_screen_rotate() {
  if (M5.BtnA.wasPressed()) {
    if (settings.rotate_screen_90_degrees) {
      settings.rotate_screen_90_degrees = false;
    } else {
      settings.rotate_screen_90_degrees = true;
    }
    save_settings();
    apply_rotation();
    M5.Display.fillScreen(settings.background_color);
    draw_screen = true;
    toggle_screen_settings = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Screen Rotate");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Rotates the screen by +90", M5.Display.width() / 2, 35);
    M5.Display.drawString("degrees when enabled", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    if (settings.rotate_screen_90_degrees) {
      M5.Display.drawString("YES", M5.Display.width() / 2, 77);
    } else {
      M5.Display.drawString("NO", M5.Display.width() / 2, 77);
    }
    settings_footer("screen rotate");
  }
}

void screen_settings_coord_format() {
  toggle_coord_format();
  if (draw_screen || toggle_screen_settings) {
    screen_header("Coordinate Format");
    show_defined_configuration(settings.coord_format);
    settings_footer("coordinate format");
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

void screen_settings_min_trip_speed() {
  if (M5.BtnA.wasPressed()) {
    settings.min_moving_speed = settings.min_moving_speed + 0.5;
    if (settings.min_moving_speed > 10) {
      settings.min_moving_speed = 0;
    }
    save_settings();
    draw_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    double converted_speed;
    if (settings.units_speed_measurement == "km/h") {
      converted_speed = settings.min_moving_speed;
    } else if (settings.units_speed_measurement == "mph") {
      converted_speed = settings.min_moving_speed * 0.621371;
    } else if (settings.units_speed_measurement == "knots") {
      converted_speed = settings.min_moving_speed * 0.539957;
    } else if (settings.units_speed_measurement == "m/s") {
      converted_speed = settings.min_moving_speed / 3.6;
    } else {
      converted_speed = settings.min_moving_speed;
    }
    String result_speed = String(converted_speed, 2) + " " + settings.units_speed_measurement;
    result_speed.trim();

    screen_header("Min. Trip Speed");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Minimum speed to record trip", M5.Display.width() / 2, 35);
    M5.Display.drawString("distance and prevent GPS drift", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    if (settings.min_moving_speed > 0) {
      M5.Display.drawString(result_speed, M5.Display.width() / 2, 77);
    } else {
      M5.Display.drawString("Disabled", M5.Display.width() / 2, 77);
    }
    settings_footer("min. trip speed");
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

void screen_settings_screen_update_interval() {
  if (M5.BtnA.wasPressed()) {
    if (settings.screen_update_interval_seconds == 0) {
      settings.screen_update_interval_seconds = 1;
    } else if (settings.screen_update_interval_seconds == 1) {
      settings.screen_update_interval_seconds = 5;
    } else if (settings.screen_update_interval_seconds == 5) {
      settings.screen_update_interval_seconds = 15;
    } else if (settings.screen_update_interval_seconds == 15) {
      settings.screen_update_interval_seconds = 30;
    } else if (settings.screen_update_interval_seconds == 30) {
      settings.screen_update_interval_seconds = 60;
    } else if (settings.screen_update_interval_seconds == 60) {
      settings.screen_update_interval_seconds = 0;
    } else {
      settings.screen_update_interval_seconds = 0;
    }
    save_settings();
    draw_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Screen Update");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Sets the screen update interval", M5.Display.width() / 2, 35);
    M5.Display.drawString("or automatic updates", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    if (settings.screen_update_interval_seconds == 0) {
      M5.Display.drawString("Automatic", M5.Display.width() / 2, 77);
    } else {
      M5.Display.drawString(String(settings.screen_update_interval_seconds) + " seconds", M5.Display.width() / 2, 77);
    }
    settings_footer("update interval");
  }
}

void screen_settings_module_timeout() {
  if (M5.BtnA.wasPressed()) {
    if (settings.gps_timeout_seconds == 0) {
      settings.gps_timeout_seconds = 5;
    } else if (settings.gps_timeout_seconds == 5) {
      settings.gps_timeout_seconds = 15;
    } else if (settings.gps_timeout_seconds == 15) {
      settings.gps_timeout_seconds = 30;
    } else if (settings.gps_timeout_seconds == 30) {
      settings.gps_timeout_seconds = 60;
    } else if (settings.gps_timeout_seconds == 60) {
      settings.gps_timeout_seconds = 0;
    } else {
      settings.gps_timeout_seconds = 15;
    }
    save_settings();
    draw_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Module Timeout");
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Shows 'Communication Warning'", M5.Display.width() / 2, 35);
    M5.Display.drawString("if no data is received in time", M5.Display.width() / 2, 50);
    M5.Display.fillRoundRect(20, 62, 200, 28, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    if (settings.gps_timeout_seconds > 0) {
      M5.Display.drawString(String(settings.gps_timeout_seconds) + " seconds", M5.Display.width() / 2, 77);
    } else {
      M5.Display.drawString("Disabled", M5.Display.width() / 2, 77);
    }
    settings_footer("module timeout");
  }
}

void screen_settings_basic_settings() {
  if (M5.BtnA.wasPressed()) {
    basic_settings();
    draw_screen = true;  // Forces the screen to redraw
    toggle_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    screen_header("Basic Settings");
    M5.Display.setTextDatum(top_center);
    M5.Display.fillRoundRect(10, 35, 220, 60, 10, settings.theme_color);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(settings.background_color);
    M5.Display.drawString("RX: " + String(settings.gpio_rx) + " - TX: " + String(settings.gpio_tx), M5.Display.width() / 2, 45);
    M5.Display.drawString("Baud Rate: " + String(settings.module_baudrate), M5.Display.width() / 2, 70);
    M5.Display.setTextColor(settings.theme_color);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Press button A to", M5.Display.width() / 2, 105);
    M5.Display.drawString("configure the basic settings", M5.Display.width() / 2, 120);
  }
}

void screen_settings_reset_trip() {
  if (M5.BtnA.wasPressed()) {  // When button A is pressed, all trip data and statistics are reset
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
    gps_trip.time_in_motion_millis = 0;
    gps_trip.last_time_in_motion_update_millis = millis();
    gps_trip.last_speed_update_millis = 0;
    gps_trip.is_moving = false;
    draw_screen = true;
  }
  if (draw_screen || toggle_screen_settings) {
    if (gps_trip.start_latitude == 0 && gps_trip.max_speed == 0 && gps_trip.max_altitude == 0) {
      screen_header("Reset Trip");
      M5.Display.setTextDatum(top_center);
      M5.Display.setTextSize(2);
      M5.Display.drawString("Trip data reset!", M5.Display.width() / 2, 65);
      M5.Display.setTextSize(1);
      M5.Display.setCursor(10, 120);
      M5.Display.print("Button B = next setting");
    } else {
      screen_header("Reset Trip");
      M5.Display.setTextDatum(top_center);
      M5.Display.setTextSize(2);
      M5.Display.drawString("Reset Trip?", M5.Display.width() / 2, 35);
      M5.Display.setTextSize(1);
      M5.Display.drawString("All trip data will be reset.", M5.Display.width() / 2, 60);
      M5.Display.drawString("New data will be recorded", M5.Display.width() / 2, 75);
      M5.Display.drawString("from the next GPS/GNSS fix.", M5.Display.width() / 2, 90);
      M5.Display.setCursor(10, 105);
      M5.Display.print("Button A = reset trip");
      M5.Display.setCursor(10, 120);
      M5.Display.print("Button B = next setting");
    }
  }
}

void screen_settings_exit() {
  if (M5.BtnA.wasPressed()) {
    save_settings();
    settings_mode = false;
    toggle_screen = true;
    draw_screen = true;
    toggle_screen_settings = true;
    screen_i = 0;
    refresh_screen_exit_settings = true;  // Forces the screen to refresh on the next loop, even without the need to press a button or receive a new update from the GPS/GNSS module
    if (screen_general_i == 0) {
      M5.Display.setRotation(horizontal_screen_rotation);  // Standard rotation (screen horizontal)
    } else {
      M5.Display.setRotation(vertical_screen_rotation);  // Vertical screen
    }
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
  M5.Display.setTextColor(settings.background_color);
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
    M5.Display.fillRect(0, 0, 240, 25, settings.theme_color);  // Draw a square
    M5.Display.setTextColor(settings.background_color);        // Set the text color to the background color
    M5.Display.setTextDatum(top_left);                         // Setting the text datum to top and left
    M5.Display.setFont(&fonts::Font0);                         // Ensure it is in the default font
    if (title.length() > 12) {                                 // If the title string has more than 12 characters, the font size will be slightly smaller and positioned a bit lower
      M5.Display.setTextSize(1.8f);                            // Setting the text size to 1.8
      M5.Display.drawString(title, 10, 6);                     // Print the title centered at the top
    } else {                                                   // If the title string is 12 characters or less, the font size will be normal
      M5.Display.setTextSize(2);                               // Setting the text size to 2
      M5.Display.drawString(title, 10, 5);                     // Print the title centered at the top
    }
    M5.Display.setTextSize(2);                                                                            // Setting the text size to 2
    M5.Display.setTextDatum(top_right);                                                                   // Setting the text datum to top and right
    if (M5.Power.isCharging() || data_comparison.battery_charging) {                                      // Check if it is charging
      M5.Display.drawString("+" + String(M5.Power.getBatteryLevel()) + "%", M5.Display.width() - 10, 5);  // Prints the + and the battery percentage
    } else {                                                                                              // If it is not charging
      M5.Display.drawString(String(M5.Power.getBatteryLevel()) + "%", M5.Display.width() - 10, 5);        // prints the battery percentage
    }
    data_comparison.battery_percentage = M5.Power.getBatteryLevel();
    data_comparison.battery_charging = M5.Power.isCharging();
  }
  M5.Display.fillRect(0, 25, 240, 110, settings.background_color);  // Draw a square with the background color
  M5.Display.setTextColor(settings.theme_color);                    // Setting the text color to settings.theme_color
  M5.Display.setTextDatum(top_left);                                // Returning the text datum to the standard
}

void screen_header_vertical(String title, bool bg_color) {  // It does the same thing as the standard function, but with the screen in portrait mode
  if (toggle_screen || toggle_screen_settings || data_comparison.battery_percentage != M5.Power.getBatteryLevel() || data_comparison.battery_charging != M5.Power.isCharging()) {
    if (bg_color) {
      M5.Display.fillRect(0, 0, M5.Display.width(), 15, settings.theme_color);
    } else {
      M5.Display.fillRect(0, 0, M5.Display.width(), 15, settings.background_color);
    }
    M5.Display.setTextSize(1);
    if (bg_color) {
      M5.Display.setTextColor(settings.background_color);
    } else {
      M5.Display.setTextColor(settings.theme_color);
    }
    M5.Display.setTextDatum(top_left);
    M5.Display.setFont(&fonts::Font0);
    M5.Display.drawString(title, 5, 4);
    M5.Display.setTextDatum(top_right);
    if (M5.Power.isCharging() || data_comparison.battery_charging) {
      M5.Display.drawString("+" + String(M5.Power.getBatteryLevel()) + "%", M5.Display.width() - 5, 4);
    } else {
      M5.Display.drawString(String(M5.Power.getBatteryLevel()) + "%", M5.Display.width() - 5, 4);
    }
    data_comparison.battery_percentage = M5.Power.getBatteryLevel();
    data_comparison.battery_charging = M5.Power.isCharging();
  }
  if (bg_color) {
    M5.Display.fillRect(0, 15, M5.Display.width(), M5.Display.height() - 15, settings.background_color);
  } else {
    if (toggle_screen) {
      M5.Display.fillRect(0, 15, M5.Display.width(), M5.Display.height() - 15, settings.theme_color);
    }
  }
  M5.Display.setTextColor(settings.theme_color);
  M5.Display.setTextDatum(top_center);
}

void basic_settings() {
  bool show_basic_settings_message = true;
  bool update_message_display = true;
  while (show_basic_settings_message) {
    M5.update();
    if (update_message_display) {
      M5.Display.fillScreen(settings.background_color);
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(settings.theme_color);
      M5.Display.setTextDatum(top_center);
      M5.Display.drawString("Basic Settings", M5.Display.width() / 2, 10);
      M5.Display.setTextSize(1);
      M5.Display.drawString("Set GPIO RX, GPIO TX, and baud rate", M5.Display.width() / 2, 40);
      M5.Display.drawString("to match your GNSS module.", M5.Display.width() / 2, 55);
      M5.Display.setTextDatum(top_left);
      M5.Display.drawString("> Button B: Change value", 10, 75);
      M5.Display.drawString("> Button A: Confirm and continue", 10, 90);
      M5.Display.setTextDatum(top_center);
      M5.Display.drawString("Press Button A to continue", M5.Display.width() / 2, 115);
    }
    update_message_display = false;
    if (M5.BtnA.wasPressed()) {
      show_basic_settings_message = false;
    }
  }
  set_gpio_rx();
  set_gpio_tx();
  set_baudrate();
  save_settings();
  if (gps_serial_initialized) {  // Checks if a serial communication has already been initiated.
    gps_serial.end();            // Closes the current UART configuration before applying the new baud rate
  }
  gps_serial.begin(settings.module_baudrate, SERIAL_8N1, settings.gpio_rx, settings.gpio_tx);  // Starts a new serial communication with the specified baud rate
  gps_serial_initialized = true;                                                               // Sets gps_serial_initialized to true. If another serial communication needs to be initiated, the variable indicates that the previous one must be terminated
  M5.Display.fillScreen(settings.background_color);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(settings.theme_color);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Basic Settings", M5.Display.width() / 2, 10);
  M5.Display.drawString("completed", M5.Display.width() / 2, 30);
  M5.Display.setTextDatum(top_left);
  M5.Display.drawString("GPIO RX: " + String(settings.gpio_rx), 15, 60);
  M5.Display.drawString("GPIO TX: " + String(settings.gpio_tx), 15, 85);
  M5.Display.drawString("Baud Rate: " + String(settings.module_baudrate), 15, 110);
  delay(3000);  // Waiting time of 3000 milliseconds
}

void set_baudrate() {
  bool undefined_baudrate = true;                                                                                                           // Control variable for the repetition loop
  bool update_baudrate_display = true;                                                                                                      // Control variable for when the screen needs to be updated
  int selected_baudrate_i = findIndex(baudrate_options, sizeof(baudrate_options) / sizeof(baudrate_options[0]), settings.module_baudrate);  // Finding the already defined baudrate
  if (selected_baudrate_i < 0) {
    selected_baudrate_i = 0;
  }
  M5.Display.fillScreen(settings.background_color);                    // Fill the screen with the background color
  M5.Display.setTextSize(2);                                           // Setting the text size to 2
  M5.Display.setTextColor(settings.theme_color);                       // Setting the text color to settings.theme_color
  M5.Display.setTextDatum(top_center);                                 // Setting the text datum to top and center
  M5.Display.drawString("Set Baud Rate", M5.Display.width() / 2, 10);  // "Set baud rate" is printed centered at the top
  M5.Display.setTextDatum(top_left);                                   // Setting the text datum to top and left
  int start_x = 10;
  int cursor_x = 10;
  int cursor_y = 50;
  int margin_x = 110;
  int margin_y = 25;
  while (undefined_baudrate) {                        // It stays in this loop as long as undefined_baudrate is true
    M5.update();                                      // Updates the state of the M5StickS3 hardware, especially buttons and input events
    if (M5.BtnB.wasPressed()) {                       // Checks if button B has been pressed
      selected_baudrate_i = selected_baudrate_i + 1;  // 1 is added to the value of selected_baudrate_i
      update_baudrate_display = true;                 // update_baudrate_display is set to true to update the display
    }
    if (selected_baudrate_i > (sizeof(baudrate_options) / sizeof(baudrate_options[0])) - 1) {  // If the value of selected_baudrate_i is equal to the number of indices in baudrate_options
      selected_baudrate_i = 0;                                                                 // selected_baudrate_i returns to 0
    }
    if (update_baudrate_display) {
      M5.Display.fillRect(0, 50, M5.Display.width(), M5.Display.height() - 50, settings.background_color);  // Draw a square on the screen
      M5.Display.drawLine(10, 50, 230, 50, settings.theme_color);
      M5.Display.drawLine(10, 75, 230, 75, settings.theme_color);
      M5.Display.drawLine(10, 100, 230, 100, settings.theme_color);
      M5.Display.drawLine(65, 125, 175, 125, settings.theme_color);
      M5.Display.drawLine(10, 50, 10, 100, settings.theme_color);
      M5.Display.drawLine(120, 50, 120, 100, settings.theme_color);
      M5.Display.drawLine(230, 50, 230, 100, settings.theme_color);
      M5.Display.drawLine(65, 100, 65, 125, settings.theme_color);
      M5.Display.drawLine(175, 100, 175, 125, settings.theme_color);
      cursor_x = 10;
      cursor_y = 50;
      M5.Display.setTextSize(2);
      M5.Display.setTextDatum(middle_center);
      for (int i_baudrate = 0; i_baudrate < sizeof(baudrate_options) / sizeof(baudrate_options[0]); i_baudrate++) {  // traverses the baudrate_options array
        if (cursor_x > 180) {
          cursor_x = start_x;
          cursor_y = cursor_y + margin_y;
        }
        if (cursor_y > 90) {
          cursor_x = start_x + (margin_x / 2);
        }
        if (selected_baudrate_i == i_baudrate) {  // If selected_baudrate_i is equal to i_baudrate
          M5.Display.fillRect(cursor_x, cursor_y, margin_x, margin_y, settings.theme_color);
          M5.Display.setTextColor(settings.background_color);
          M5.Display.drawString(String(baudrate_options[i_baudrate]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          M5.Display.setTextColor(settings.theme_color);
        } else {  // If selected_baudrate_i is not equal to i_baudrate
          M5.Display.drawString(String(baudrate_options[i_baudrate]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
        }
        cursor_x = cursor_x + margin_x;
      }
      update_baudrate_display = false;  // update_baudrate_display is set to false to avoid unnecessary display updates
    }
    if (M5.BtnA.wasPressed()) {                                          // Checks if button A has been pressed
      settings.module_baudrate = baudrate_options[selected_baudrate_i];  // The module_baudrate will be the selected_baudrate_i index of the baudrate_options array
      undefined_baudrate = false;                                        // undefined_baudrate is set to false to exit the loop
      cursor_x = 10;
      cursor_y = 50;
      for (int i_baudrate = 0; i_baudrate < sizeof(baudrate_options) / sizeof(baudrate_options[0]); i_baudrate++) {  // traverses the baudrate_options array
        if (cursor_x > 180) {
          cursor_x = start_x;
          cursor_y = cursor_y + margin_y;
        }
        if (cursor_y > 90) {
          cursor_x = start_x + (margin_x / 2);
        }
        if (selected_baudrate_i == i_baudrate) {  // If selected_baudrate_i is equal to i_baudrate
          M5.Display.fillRect(cursor_x, cursor_y, margin_x, margin_y, settings.theme_color);
          M5.Display.setTextColor(settings.background_color);
          M5.Display.drawString(String(baudrate_options[i_baudrate]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          M5.Display.setTextColor(settings.theme_color);
        } else {  // If selected_baudrate_i is not equal to i_baudrate
          M5.Display.fillRect(cursor_x + 1, cursor_y + 1, margin_x - 2, margin_y - 2, settings.background_color);
        }
        cursor_x = cursor_x + margin_x;
      }
      delay(1000);
    }
  }
}

void set_gpio_rx() {
  bool undefined_gpio_rx = true;
  bool update_gpio_rx_display = true;
  int selected_gpio_rx_i = findIndex(gpio_options, sizeof(gpio_options) / sizeof(gpio_options[0]), settings.gpio_rx);
  if (selected_gpio_rx_i < 0) {
    selected_gpio_rx_i = 0;
  }
  M5.Display.fillScreen(settings.background_color);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(settings.theme_color);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Set GPIO RX", M5.Display.width() / 2, 10);
  M5.Display.setTextSize(1);
  M5.Display.drawString("GPS/GNSS module TX -> GPIO RX", M5.Display.width() / 2, 35);
  int start_x = 10;
  int cursor_x = 10;
  int cursor_y = 50;
  int margin_x = 55;
  int margin_y = 25;
  while (undefined_gpio_rx) {
    M5.update();
    if (M5.BtnB.wasPressed()) {
      selected_gpio_rx_i = selected_gpio_rx_i + 1;
      update_gpio_rx_display = true;
    }
    if (selected_gpio_rx_i > (sizeof(gpio_options) / sizeof(gpio_options[0])) - 1) {
      selected_gpio_rx_i = 0;
    }
    if (update_gpio_rx_display) {
      M5.Display.fillRect(0, 50, M5.Display.width(), M5.Display.height() - 50, settings.background_color);
      M5.Display.drawLine(10, 50, M5.Display.width() - 10, 50, settings.theme_color);
      M5.Display.drawLine(10, 75, M5.Display.width() - 10, 75, settings.theme_color);
      M5.Display.drawLine(10, 100, M5.Display.width() - 10, 100, settings.theme_color);
      M5.Display.drawLine(10, 125, M5.Display.width() - 10, 125, settings.theme_color);
      M5.Display.drawLine(10, 50, 10, 125, settings.theme_color);
      M5.Display.drawLine(65, 50, 65, 125, settings.theme_color);
      M5.Display.drawLine(120, 50, 120, 125, settings.theme_color);
      M5.Display.drawLine(175, 50, 175, 125, settings.theme_color);
      M5.Display.drawLine(230, 50, 230, 125, settings.theme_color);
      cursor_x = 10;
      cursor_y = 50;
      M5.Display.setTextSize(2);
      M5.Display.setTextDatum(middle_center);
      for (int i_gpio_rx = 0; i_gpio_rx < sizeof(gpio_options) / sizeof(gpio_options[0]); i_gpio_rx++) {
        if (cursor_x > 180) {
          cursor_x = start_x;
          cursor_y = cursor_y + margin_y;
        }
        if (selected_gpio_rx_i == i_gpio_rx) {
          M5.Display.fillRect(cursor_x, cursor_y, margin_x, margin_y, settings.theme_color);
          M5.Display.setTextColor(settings.background_color);
          M5.Display.drawString("G " + String(gpio_options[i_gpio_rx]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          M5.Display.setTextColor(settings.theme_color);
        } else {
          M5.Display.drawString("G " + String(gpio_options[i_gpio_rx]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
        }
        cursor_x = cursor_x + margin_x;
      }
      update_gpio_rx_display = false;
    }
    if (M5.BtnA.wasPressed()) {
      settings.gpio_rx = gpio_options[selected_gpio_rx_i];
      undefined_gpio_rx = false;
      cursor_x = 10;
      cursor_y = 50;
      for (int i_gpio_rx = 0; i_gpio_rx < sizeof(gpio_options) / sizeof(gpio_options[0]); i_gpio_rx++) {
        if (cursor_x > 180) {
          cursor_x = start_x;
          cursor_y = cursor_y + margin_y;
        }
        if (settings.gpio_rx == gpio_options[i_gpio_rx]) {
          M5.Display.fillRect(cursor_x, cursor_y, margin_x, margin_y, settings.theme_color);
          M5.Display.setTextColor(settings.background_color);
          M5.Display.drawString("G " + String(gpio_options[i_gpio_rx]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          M5.Display.setTextColor(settings.theme_color);
        } else {
          M5.Display.fillRect(cursor_x + 1, cursor_y + 1, margin_x - 2, margin_y - 2, settings.background_color);
        }
        cursor_x = cursor_x + margin_x;
      }
      delay(1000);
    }
  }
}

void set_gpio_tx() {
  bool undefined_gpio_tx = true;
  bool update_gpio_tx_display = true;
  int selected_gpio_tx_i = findIndex(gpio_options, sizeof(gpio_options) / sizeof(gpio_options[0]), settings.gpio_tx);
  if (selected_gpio_tx_i < 0) {
    selected_gpio_tx_i = 0;
  }
  M5.Display.fillScreen(settings.background_color);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(settings.theme_color);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Set GPIO TX", M5.Display.width() / 2, 10);
  M5.Display.setTextSize(1);
  M5.Display.drawString("GPIO TX -> GPS/GNSS module RX", M5.Display.width() / 2, 35);
  int start_x = 10;
  int cursor_x = 10;
  int cursor_y = 50;
  int margin_x = 55;
  int margin_y = 25;
  while (undefined_gpio_tx) {
    M5.update();
    if (M5.BtnB.wasPressed()) {
      selected_gpio_tx_i = selected_gpio_tx_i + 1;
      update_gpio_tx_display = true;
    }
    if (selected_gpio_tx_i > (sizeof(gpio_options) / sizeof(gpio_options[0])) - 1) {
      selected_gpio_tx_i = 0;
    }
    if (gpio_options[selected_gpio_tx_i] == settings.gpio_rx) {
      selected_gpio_tx_i = selected_gpio_tx_i + 1;
    }
    if (selected_gpio_tx_i > (sizeof(gpio_options) / sizeof(gpio_options[0])) - 1) {
      selected_gpio_tx_i = 0;
    }
    if (update_gpio_tx_display) {
      M5.Display.fillRect(0, 50, M5.Display.width(), M5.Display.height() - 50, settings.background_color);
      M5.Display.drawLine(10, 50, M5.Display.width() - 10, 50, settings.theme_color);
      M5.Display.drawLine(10, 75, M5.Display.width() - 10, 75, settings.theme_color);
      M5.Display.drawLine(10, 100, M5.Display.width() - 10, 100, settings.theme_color);
      M5.Display.drawLine(10, 125, M5.Display.width() - 10, 125, settings.theme_color);
      M5.Display.drawLine(10, 50, 10, 125, settings.theme_color);
      M5.Display.drawLine(65, 50, 65, 125, settings.theme_color);
      M5.Display.drawLine(120, 50, 120, 125, settings.theme_color);
      M5.Display.drawLine(175, 50, 175, 125, settings.theme_color);
      M5.Display.drawLine(230, 50, 230, 125, settings.theme_color);
      cursor_x = 10;
      cursor_y = 50;
      M5.Display.setTextSize(2);
      M5.Display.setTextDatum(middle_center);
      for (int i_gpio_tx = 0; i_gpio_tx < sizeof(gpio_options) / sizeof(gpio_options[0]); i_gpio_tx++) {
        if (cursor_x > 180) {
          cursor_x = start_x;
          cursor_y = cursor_y + margin_y;
        }
        if (selected_gpio_tx_i == i_gpio_tx) {
          M5.Display.fillRect(cursor_x, cursor_y, margin_x, margin_y, settings.theme_color);
          M5.Display.setTextColor(settings.background_color);
          M5.Display.drawString("G " + String(gpio_options[i_gpio_tx]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          M5.Display.setTextColor(settings.theme_color);
        } else {
          if (settings.gpio_rx == gpio_options[i_gpio_tx]) {
            M5.Display.drawString("RX", cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          } else {
            M5.Display.drawString("G " + String(gpio_options[i_gpio_tx]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          }
        }
        cursor_x = cursor_x + margin_x;
      }
      update_gpio_tx_display = false;
    }
    if (M5.BtnA.wasPressed()) {
      settings.gpio_tx = gpio_options[selected_gpio_tx_i];
      undefined_gpio_tx = false;
      cursor_x = 10;
      cursor_y = 50;
      for (int i_gpio_tx = 0; i_gpio_tx < sizeof(gpio_options) / sizeof(gpio_options[0]); i_gpio_tx++) {
        if (cursor_x > 180) {
          cursor_x = start_x;
          cursor_y = cursor_y + margin_y;
        }
        if (settings.gpio_tx == gpio_options[i_gpio_tx]) {
          M5.Display.fillRect(cursor_x, cursor_y, margin_x, margin_y, settings.theme_color);
          M5.Display.setTextColor(settings.background_color);
          M5.Display.drawString("G " + String(gpio_options[i_gpio_tx]), cursor_x + (margin_x / 2) + 2, cursor_y + (margin_y / 2) + 2);
          M5.Display.setTextColor(settings.theme_color);
        } else {
          M5.Display.fillRect(cursor_x + 1, cursor_y + 1, margin_x - 2, margin_y - 2, settings.background_color);
        }
        cursor_x = cursor_x + margin_x;
      }
      delay(1000);
    }
  }
}

void apply_rotation() {
  if (settings.flip_screen_horizontally) {
    if (settings.rotate_screen_90_degrees) {
      horizontal_screen_rotation = 2;
    } else {
      horizontal_screen_rotation = 1;
    }
  } else {
    if (settings.rotate_screen_90_degrees) {
      horizontal_screen_rotation = 0;
    } else {
      horizontal_screen_rotation = 3;
    }
  }
  if (settings.flip_screen_vertically) {
    if (settings.rotate_screen_90_degrees) {
      vertical_screen_rotation = 3;
    } else {
      vertical_screen_rotation = 2;
    }
  } else {
    if (settings.rotate_screen_90_degrees) {
      vertical_screen_rotation = 1;
    } else {
      vertical_screen_rotation = 0;
    }
  }
  M5.Display.setRotation(horizontal_screen_rotation);  // Updates the screen rotation
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

String format_millis_in_motion_clock(unsigned long elapsed_millis, bool subtract_current_millis, bool show_full) {
  unsigned long elapsed_seconds = elapsed_millis / 1000;
  if (subtract_current_millis) {
    elapsed_seconds = (millis() - elapsed_millis) / 1000;
  }
  unsigned long elapsed_hours = elapsed_seconds / 3600;
  unsigned long elapsed_minutes = (elapsed_seconds % 3600) / 60;
  unsigned long elapsed_secs = elapsed_seconds % 60;
  if (show_full) {
    return (elapsed_hours < 10 ? "0" : "") + String(elapsed_hours) + ":" + (elapsed_minutes < 10 ? "0" : "") + String(elapsed_minutes) + ":" + (elapsed_secs < 10 ? "0" : "") + String(elapsed_secs);
  } else {
    if (elapsed_hours > 0) {
      return (elapsed_hours < 10 ? "0" : "") + String(elapsed_hours) + ":" + (elapsed_minutes < 10 ? "0" : "") + String(elapsed_minutes) + ":" + (elapsed_secs < 10 ? "0" : "") + String(elapsed_secs);
    } else {
      return (elapsed_minutes < 10 ? "0" : "") + String(elapsed_minutes) + ":" + (elapsed_secs < 10 ? "0" : "") + String(elapsed_secs);
    }
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
  settings.gpio_rx = gpio_options[11];
  settings.gpio_tx = gpio_options[10];
  settings.theme_color = theme_color_options[0];
  settings.background_color = theme_color_options[1];
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
  settings.screen_update_interval_seconds = 0;
  settings.min_moving_speed = 1.0;
  settings.flip_screen_horizontally = false;
  settings.flip_screen_vertically = false;
  settings.rotate_screen_90_degrees = false;
}

void save_settings() {  // Saves the current configuration to NVS so it survives reboot and power loss
  preferences.begin(SETTINGS_NAMESPACE, false);
  preferences.putBool("initialized", true);
  preferences.putInt("baudrate", settings.module_baudrate);
  preferences.putInt("gpio_rx", settings.gpio_rx);
  preferences.putInt("gpio_tx", settings.gpio_tx);
  preferences.putUShort("theme_color", settings.theme_color);
  preferences.putUShort("bg_color", settings.background_color);
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
  preferences.putInt("scr_upd_int", settings.screen_update_interval_seconds);
  preferences.putDouble("min_speed", settings.min_moving_speed);
  preferences.putBool("flip_s_h", settings.flip_screen_horizontally);
  preferences.putBool("flip_s_v", settings.flip_screen_vertically);
  preferences.putBool("rotate_s_90", settings.rotate_screen_90_degrees);
  preferences.end();
}

void load_settings() {                                           // Loads the saved firmware settings from persistent storage
  preferences.begin(SETTINGS_NAMESPACE, true);                   // Opens the settings namespace in read-only mode
  bool initialized = preferences.getBool("initialized", false);  // Checks whether saved settings exist, using false as the default value
  if (!initialized) {                                            // Checks if the settings have not been initialized yet
    // No persistent settings were found, so initialize defaults and require the user to select the GNSS baud rate
    preferences.end();       // Closes the Preferences namespace
    set_default_settings();  // Loads the default firmware settings
    basic_settings();        // Calls the function that defines the initial settings: baud rate, GPIO RX, and GPIO TX
    save_settings();         // Saves the settings
    return;
  }
  settings.module_baudrate = preferences.getInt("baudrate", baudrate_options[0]);
  settings.gpio_rx = preferences.getInt("gpio_rx", gpio_options[11]);
  settings.gpio_tx = preferences.getInt("gpio_tx", gpio_options[10]);
  settings.theme_color = preferences.getUShort("theme_color", theme_color_options[0]);
  settings.background_color = preferences.getUShort("bg_color", theme_color_options[1]);
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
  settings.screen_update_interval_seconds = preferences.getInt("scr_upd_int", 0);
  settings.min_moving_speed = preferences.getDouble("min_speed", 1.0);
  settings.flip_screen_horizontally = preferences.getBool("flip_s_h", false);
  settings.flip_screen_vertically = preferences.getBool("flip_s_v", false);
  settings.rotate_screen_90_degrees = preferences.getBool("rotate_s_90", false);
  preferences.end();
}

bool validate_settings() {     // Validates settings loaded from NVS and replaces invalid values with safe defaults
  bool settings_valid = true;  // Returns true when all settings are valid, otherwise false after correcting invalid values
  bool valid_baudrate = false;
  for (int i = 0; i < sizeof(baudrate_options) / sizeof(baudrate_options[0]); i++) {
    if (settings.module_baudrate == baudrate_options[i]) {
      valid_baudrate = true;
      break;
    }
  }
  if (!valid_baudrate) {
    settings.module_baudrate = baudrate_options[0];
    settings_valid = false;
  }
  bool valid_gpio_rx = false;
  for (int i = 0; i < sizeof(gpio_options) / sizeof(gpio_options[0]); i++) {
    if (settings.gpio_rx == gpio_options[i]) {
      valid_gpio_rx = true;
      break;
    }
  }
  if (!valid_gpio_rx) {
    settings.gpio_rx = gpio_options[11];
    settings_valid = false;
  }
  bool valid_gpio_tx = false;
  for (int i = 0; i < sizeof(gpio_options) / sizeof(gpio_options[0]); i++) {
    if (settings.gpio_tx == gpio_options[i]) {
      valid_gpio_tx = true;
      break;
    }
  }
  if (!valid_gpio_tx) {
    settings.gpio_tx = gpio_options[10];
    settings_valid = false;
  }
  if (settings.gpio_rx == settings.gpio_tx) {
    settings.gpio_rx = gpio_options[11];
    settings.gpio_tx = gpio_options[10];
    settings_valid = false;
  }
  bool valid_theme_color = false;
  for (int i = 0; i < sizeof(theme_color_options) / sizeof(theme_color_options[0]); i++) {
    if (settings.theme_color == theme_color_options[i]) {
      valid_theme_color = true;
      break;
    }
  }
  if (!valid_theme_color) {
    settings.theme_color = theme_color_options[0];
    settings_valid = false;
  }
  bool valid_background_color = false;
  for (int i = 0; i < sizeof(theme_color_options) / sizeof(theme_color_options[0]); i++) {
    if (settings.background_color == theme_color_options[i]) {
      valid_background_color = true;
      break;
    }
  }
  if (!valid_background_color) {
    settings.background_color = theme_color_options[1];
    settings_valid = false;
  }
  if (settings.theme_color == settings.background_color) {
    settings.theme_color = theme_color_options[0];
    settings.background_color = theme_color_options[1];
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
  for (int i = 0; i < sizeof(timezone_options) / sizeof(timezone_options[0]); i++) {
    if (settings.timezone == timezone_options[i]) {
      valid_timezone = true;
      break;
    }
  }
  if (!valid_timezone) {
    settings.timezone = timezone_options[14];
    settings_valid = false;
  }
  if (settings.time_format != "24-hour" && settings.time_format != "12-hour") {
    settings.time_format = "24-hour";
    settings_valid = false;
  }
  if (settings.date_format != "YYYY-MM-DD" && settings.date_format != "MM/DD/YYYY" && settings.date_format != "DD/MM/YYYY") {
    settings.date_format = "YYYY-MM-DD";
    settings_valid = false;
  }
  if (settings.screen_brightness < 10 || settings.screen_brightness > 100 || settings.screen_brightness % 10 != 0) {
    settings.screen_brightness = 100;
    settings_valid = false;
  }
  bool valid_screen_timeout = false;
  for (int i = 0; i < sizeof(screen_timeout_options) / sizeof(screen_timeout_options[0]); i++) {
    if (settings.screen_timeout_seconds == screen_timeout_options[i]) {
      valid_screen_timeout = true;
      break;
    }
  }
  if (!valid_screen_timeout) {
    settings.screen_timeout_seconds = screen_timeout_options[0];
    settings_valid = false;
  }
  if (settings.gps_timeout_seconds < 0 || settings.gps_timeout_seconds > 300) {
    settings.gps_timeout_seconds = 15;
    settings_valid = false;
  }
  if (settings.screen_update_interval_seconds < 0 || settings.screen_update_interval_seconds > 300) {
    settings.screen_update_interval_seconds = 0;
    settings_valid = false;
  }
  if (settings.min_moving_speed < 0.0 || settings.min_moving_speed > 100.0) {
    settings.min_moving_speed = 1.0;
    settings_valid = false;
  }
  return settings_valid;
}