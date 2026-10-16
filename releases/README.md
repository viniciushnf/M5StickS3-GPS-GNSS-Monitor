# GPS & GNSS Monitor — Firmware Releases

This directory contains the published versions of **GPS & GNSS Monitor** for the M5StickS3. Each release includes the Arduino source code (`.ino`) and its precompiled firmware binary (`.bin`), allowing users to inspect the implementation or install a released version.

## Version History

### Version 1.1 — Interface and coordinate-display improvements

**Release type:** Feature update

Version 1.1 expands the display options and improves how key navigation information is presented on the M5StickS3.

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

**Release type:** Initial public release

Version 1.0 established the core GPS & GNSS Monitor firmware for the M5StickS3. It receives GNSS data from a compatible module over UART and presents navigation and receiver information through the device's screen-based interface.

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