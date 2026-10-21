# GPS & GNSS Monitor — Firmware Releases

This directory contains the published versions of **GPS & GNSS Monitor** for the M5StickS3. Each release includes the Arduino source code (`.ino`) and its precompiled firmware binary (`.bin`), allowing users to inspect the implementation or install a released version.

## Version History

### Version 1.2 — Expanded configuration and display customization

Version 1.2 significantly expands the configuration capabilities of GPS & GNSS Monitor, introducing customizable UART pins, advanced display controls, new color options, improved communication monitoring, and enhanced trip statistics. This release also redesigns several screens and makes all settings stored in NVS editable through the firmware interface.

**Version released on:** October 21, 2026.

#### Added

- **Configurable UART GPIOs:** RX and TX pins can now be selected independently through the new Basic Settings screen, alongside the baud rate. This allows users to configure the serial connection according to their hardware setup without modifying the firmware source code. GPIOs available to be configured as RX and TX: 1, 2, 9, 10, 13, 15, 26, 32, 33, 36, 43 and 44.

- **Screen Update:** A new setting allows users to control how frequently the display refreshes. Available options are Automatic, 1 second, 5 seconds, 15 seconds, 30 seconds, and 60 seconds. In Automatic mode, the screen updates whenever new data is received from the GPS/GNSS module. Button interactions, such as navigating between screens, trigger immediate updates regardless of the selected interval.

- **Background Color:** Added a dedicated setting for selecting the display background color, allowing users to customize the screen appearance beyond the previously fixed black background.

- **Expanded Color Palette:** Added BLACK, ORANGE, PURPLE, PINK, NAVY, BROWN, and GRAY to the available colors. The complete palette now includes WHITE, BLACK, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, PURPLE, PINK, NAVY, BROWN, and GRAY. These colors can be used for both Theme Color and Background Color.

- **Screen Flip H and Screen Flip V:** Added independent horizontal and vertical screen-flipping options, each rotating the display by 180 degrees along the corresponding axis. These settings provide additional display orientations for different mounting and usage configurations.

- **Screen Rotate:** Added an optional setting that applies an additional 90-degree adjustment to `M5.Display.setRotation()`. This helps correct display orientation issues and simplifies adapting the firmware to other compatible devices with different screen orientations.

- **Min. Trip Speed:** The minimum speed required to accumulate trip distance and Time in Motion is now configurable from 0 to 10 km/h in 0.5 km/h increments. Previously, this threshold was fixed at 1 km/h.

- **Time in Motion:** Added a dedicated movement-time counter that accumulates time only while the current speed is equal to or greater than the configured minimum trip speed. This provides a more accurate representation of time spent moving, independently of the total trip duration.

- **Module Timeout:** Added a configurable timeout for the Communication Warning screen. Available options are Disabled, 5 seconds, 15 seconds, 30 seconds, and 60 seconds. When enabled, the warning appears after the selected period without communication from the GPS/GNSS module. When Disabled, the communication warning is suppressed regardless of how long communication is interrupted.

- **Screen Timeout Information:** Added a brief explanation of the Screen Timeout functionality to its settings screen, making the behavior easier to understand.

- **Extended About Screen:** Added a second About screen, accessible by pressing Button A, displaying the developer information and a link to the firmware's GitHub repository.

#### Changed and Improved

- **Basic Settings:** Replaced the previous Baud Rate settings screen with Basic Settings, which displays the currently selected RX GPIO, TX GPIO, and baud rate. Pressing Button A opens the configuration interface for these three parameters.

- **Redesigned Settings Screens:** Updated the visual presentation of the baud rate selection and Screen Timeout screens. The Color settings screen was renamed Theme Color to distinguish it from the new Background Color setting.

- **Session Info:** Redesigned the Session Info screen to display Time in Motion, Trip Time, and Fix Time, providing a clearer overview of the current session.

- **Route Stats:** Replaced TRIP TIME with TIME IN MOTION on the third Route Stats screen, making the displayed statistic consistent with the new movement-time tracking functionality.

- **Editable Persistent Settings:** All configuration values stored in ESP32 NVS can now be edited through the settings menu, providing greater flexibility when adjusting the firmware configuration.

- **Startup Screen:** Expanded the startup screen, displayed for three seconds when the device initializes, to include the firmware name, version, and developer's GitHub link.

- **Adaptive Screen Headers:** Screen titles containing more than 12 characters now use a slightly smaller font to improve readability and help accommodate longer titles.

Release files:

- Source code: `GPS_GNSS_Monitor-v1.2.ino`
- Compiled firmware: `GPS_GNSS_Monitor-v1.2.bin`

---

### Version 1.1 — Interface and coordinate-display improvements

Version 1.1 expands the display options and improves how key navigation information is presented on the M5StickS3.

**Version released on:** October 16, 2026.

#### Added

- **New General sub-screens:** Three additional portrait layouts provide alternative views of speed, altitude, satellite information, HDOP, date, time, coordinates, and course.
- **New Route Stats sub-screens:** Two additional portrait layouts provide dedicated views of speed statistics, accumulated route distance, straight-line distance, and trip time.
- **Multiple coordinate formats in General:** Latitude and longitude can now be displayed in Decimal Degrees (Decimal), Degrees and Decimal Minutes (DM), or Degrees, Minutes, and Seconds (DMS), according to the selected coordinate format.
- **Graphical speed comparison:** A new speed indicator visually compares current speed with average, minimum, and maximum recorded speeds.
- **Improved visual presentation:** New fonts, larger numerical values, and better-organized information make key GPS/GNSS data easier to read.

**Release files:**

- Source code: `GPS_GNSS_Monitor-v1.1.ino`
- Compiled firmware: `GPS_GNSS_Monitor-v1.1.bin`

---

### Version 1.0 — Initial release

Version 1.0 established the core GPS & GNSS Monitor firmware for the M5StickS3. It receives GNSS data from a compatible module over UART and presents navigation and receiver information through the device's screen-based interface.

**Version released on:** September 6, 2026.

#### Included features

- **Live GNSS information:** latitude, longitude, altitude, speed, course/direction, satellite count, HDOP/position-quality information, date, and time.
- **Trip and route statistics:** trip distance, accumulated route distance, moving time, average moving speed, minimum and maximum speed, and minimum and maximum altitude.
- **Movement filtering:** route distance and moving time are only accumulated when reported speed exceeds the configured minimum moving-speed threshold, reducing false movement caused by small position variations while stationary.
- **Configurable measurements and presentation:** coordinate format, speed units, altitude units, distance units, theme color, display brightness, date format, time format, and timezone.
- **GNSS communication configuration:** selectable UART baud rate and a configurable communication timeout warning.
- **Persistent settings:** configuration is stored in ESP32 NVS, so settings can persist through restarts and power cycles. Invalid stored values are replaced with defaults.
- **On-device navigation:** the buttons are used to move between screens and interact with settings and screen-specific options.

#### Added

- The initial set of monitoring screens, GNSS data presentation, trip/route statistics, configurable settings, and persistent configuration storage.

**Release files:**

- Source code: `GPS_GNSS_Monitor-v1.0.ino`
- Compiled firmware: `GPS_GNSS_Monitor-v1.0.bin`

---

## File Types

- **`.ino` — Source code:** use this file to review or modify the firmware, or to compile and upload it through the Arduino IDE with the required M5StickS3 board support and libraries installed.
- **`.bin` — Compiled firmware:** use this file to flash the corresponding precompiled release. Follow the installation instructions and any flashing requirements documented in the main repository README.