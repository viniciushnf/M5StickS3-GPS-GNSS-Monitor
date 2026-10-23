<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/cover.webp" alt="GPS & GNSS Monitor cover" width="100%">
</p>

# GPS & GNSS Monitor for M5StickS3

GPS & GNSS Monitor turns the M5Stack StickS3 into a compact, portable device for monitoring data received from a compatible GPS/GNSS receiver over UART. It displays position, satellite reception, speed, altitude, course, date and time, as well as trip and route statistics.

The firmware is intended for portable navigation, GPS/GNSS module testing, field monitoring, positioning experiments, and learning about GNSS data. Most settings can be changed directly on the device and are saved in non-volatile storage, so you do not need to edit the source code for everyday configuration.

## 🚀 Purpose and Features

The firmware receives and parses GPS/GNSS NMEA data and presents it on the StickS3 display. Its main features include:

- **Live GPS/GNSS data:** position, satellite count, HDOP, speed, altitude, course, date and time.
- **Position formats:** decimal degrees, degrees/minutes/seconds (DMS), and degrees/decimal minutes (DM).
- **Position Quality:** a firmware-generated description based on HDOP.
- **Trip and route statistics:** straight-line distance from the starting point, accumulated route distance, average moving speed, minimum and maximum speed, minimum and maximum altitude, and time in motion.
- **Session timers:** time in motion, trip time, and time since the first valid GPS/GNSS fix.
- **Configurable communication:** UART RX/TX GPIO selection and baud rate.
- **Display customization:** theme and background colors, brightness, screen timeout, and screen orientation.
- **Configurable screen refresh:** automatic updates or fixed intervals.
- **Communication warning:** an optional warning if new module data stops arriving.
- **Configurable units and time display:** speed, altitude, distance, coordinate format, date format, time format, and timezone.
- **Persistent settings:** configuration is saved in the ESP32's NVS and retained after restart.
- **Button-based controls:** navigate screens and change settings without a computer.

---

## 🖥️ Screens

The firmware has main monitoring screens and a separate Settings menu. Button B navigates between screens, while Button A performs the action associated with the current screen.

### Main monitoring screens

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens.gif" alt="Main firmware screens" width="70%">
</p>

| Screen | Description |
|---|---|
| **General** | An overview of GPS/GNSS status, including satellite information and Position Quality. It has one landscape view and three portrait views. |
| **Position** | Shows the current latitude and longitude using the selected coordinate format. |
| **Speed** | Shows the speed reported by the GPS/GNSS module, using the selected speed unit, and includes Position Quality. |
| **Altitude** | Shows the altitude reported by the module using meters or feet, and includes Position Quality. |
| **Distance** | Shows the straight-line distance between the trip starting position and the current position. This is not the accumulated distance along the route. |
| **Route Stats** | Shows accumulated route distance, average moving speed, minimum and maximum speed, and time in motion. It has one landscape view and two portrait views. |
| **Course** | Shows the direction of travel calculated from GPS/GNSS course data. Course is based on movement, not a magnetic compass. |
| **Time** | Shows the date and time received from the module, adjusted using the configured timezone. |
| **Session Info** | Shows Time in Motion, Trip Time, and Fix Time. |
| **About** | Shows firmware information, including the version, selected RX/TX GPIOs, and baud rate. Button A switches to a second view with developer information and the GitHub repository link. |
| **Settings** | Allows access to the settings screens. |

#### Navigating the main screens

- **Button B — short press:** go to the next main screen.
- **Button B — long press:** go to the previous main screen.
- **Button A on General:** cycle through its available layouts, including the portrait views.
- **Button A on Route Stats:** cycle through its available layouts, including the portrait views.
- **Button A on About:** switch between the two About views.
- Button A has screen-specific actions on other screens; for example, it can change a displayed format or turn the display back on after a timeout.

### Settings screens

The Settings menu lets you customize the display, communication, measurement formats, and trip tracking.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-settings.gif" alt="Settings screens" width="70%">
</p>

| Setting | Description |
|---|---|
| **Theme Color** | Selects the color used for interface elements, text, and indicators. |
| **Background Color** | Selects the display background color. |
| **Brightness** | Adjusts display brightness from 10% to 100%. |
| **Screen Timeout** | Sets how long the display waits without button interaction before turning off, or disables the timeout. |
| **Screen Flip H** | Rotates horizontal layouts by 180 degrees when enabled. |
| **Screen Flip V** | Rotates vertical layouts by 180 degrees when enabled. |
| **Screen Rotate** | Applies an additional 90-degree rotation. |
| **Coord. Format** | Selects Decimal, DMS, or DM coordinate formatting. |
| **Speed Unit** | Selects km/h, mph, knots, or m/s. |
| **Min. Trip Speed** | Sets the minimum reported speed used for trip distance and time-in-motion calculations. |
| **Altitude Unit** | Selects meters or feet. |
| **Distance Unit** | Selects Metric or Imperial distance display. |
| **Timezone** | Sets the UTC offset used to display local date and time. |
| **Date Format** | Selects YYYY-MM-DD, MM/DD/YYYY, or DD/MM/YYYY. |
| **Time Format** | Selects 24-hour or 12-hour time. |
| **Screen Update** | Selects automatic screen updates or a fixed refresh interval. |
| **Module Timeout** | Sets when the Communication Warning appears, or disables the warning. |
| **Basic Settings** | Displays and configures the UART RX GPIO, TX GPIO, and module baud rate. |
| **Reset Trip** | Resets trip-related timers, distances, and statistics. |
| **Exit** | Leaves Settings and returns to the General screen. |

#### Navigating Settings

- **Button B — short press:** move to the next setting.
- **Button B — long press:** move to the previous setting.
- **Button A:** change, select, or confirm the current setting.
- The **Basic Settings** workflow uses Button B to cycle through choices and Button A to confirm each choice.

---

## 🎛️ Features and Configuration Guide

### ➡️ Display colors, brightness, and orientation

**Theme Color** controls the color of text, interface elements, and visual indicators. **Background Color** controls the background. Both settings offer these 14 colors:

`WHITE`, `BLACK`, `RED`, `GREEN`, `BLUE`, `YELLOW`, `CYAN`, `MAGENTA`, `ORANGE`, `PURPLE`, `PINK`, `NAVY`, `BROWN`, `GRAY`

The theme and background colors cannot be set to the same color, which helps keep the screen readable.

**Brightness** can be set from 10% to 100%. A lower brightness may make the display more comfortable in dark environments and can reduce display power consumption.

The orientation options are:

- **Screen Flip H:** rotates horizontal screens by 180 degrees.
- **Screen Flip V:** rotates vertical screens by 180 degrees.
- **Screen Rotate:** applies an additional 90-degree rotation.

These options help correct the display orientation for different ways of mounting or holding the device. They are especially useful if the StickS3 or a compatible device is installed in an orientation that differs from the usual one.

### ➡️ Portrait layouts for General and Route Stats

Both **General** and **Route Stats** include portrait-oriented layouts in addition to their landscape layout. When either screen is open, press **Button A** to cycle through its available layouts.

General has four layouts in total: one landscape layout and three portrait layouts. Route Stats has three layouts: one landscape layout and two portrait layouts.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-general.gif" alt="General screen layouts" width="30%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-route-stats.gif" alt="Route Stats screen layouts" width="30%">
</p>

### ➡️ Trip and route statistics are estimates

The firmware calculates several useful trip values from the data received from the GPS/GNSS module:

- **Route Distance:** adds the distances between consecutive reported positions while the firmware considers the device to be moving.
- **Average Moving Speed:** estimates average speed during qualifying movement, rather than averaging in all time spent stationary.
- **Time in Motion:** accumulates time while reported speed meets the configured Min. Trip Speed.
- **Minimum and Maximum Speed:** records the lowest and highest qualifying speed values used by the trip statistics.
- **Minimum and Maximum Altitude:** records the minimum and maximum altitude values tracked for the current trip.
- **Speed indicator bar:** on the second Route Stats portrait view, the indicator compares current speed with the recorded minimum, average, and maximum speeds. Its position varies between the minimum and maximum values, with the average represented around the middle reference point.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/route-stats-speed.gif" alt="Speed indicator bar" width="30%">
</p>

These values are **estimates, not precision surveying measurements**. GNSS positions naturally fluctuate, and accuracy depends on the receiver, antenna, satellite geometry, signal quality, surroundings, obstructions, and other conditions. Small position errors can affect accumulated route distance and other statistics. The Min. Trip Speed threshold can reduce unwanted movement detection while stationary, but it cannot eliminate every source of error.

### ➡️ Minimum Trip Speed

**Min. Trip Speed** defines the minimum GPS/GNSS-reported speed at which the firmware considers the device to be moving for selected trip calculations. The available setting ranges from **0 to 10 km/h in 0.5 km/h increments**; the displayed value follows the selected speed unit.

When the reported speed is equal to or greater than this threshold, the firmware can accumulate **Time in Motion** and **Route Distance**, and update related moving-speed statistics such as average, minimum, and maximum speed. If reported speed is below the threshold, these movement-based calculations do not accumulate for that interval.

A higher threshold can help prevent small GNSS position changes from being counted as travel while the device is stationary. A lower threshold allows slower movement to count. This setting does **not** change the speed reported by the GPS/GNSS module itself.

### ➡️ Straight-line distance — Distance screen

The **Distance** screen measures the straight-line distance from the trip's starting position to the current position. When trip tracking begins, the firmware stores a reference position and calculates the distance from that point as new position data arrives.

This is different from Route Distance. If you travel along winding roads or walk around an area and return close to the starting point, the straight-line distance can be small even though the actual route was much longer.

### ➡️ Session Info: Time in Motion, Trip Time, and Fix Time

The **Session Info** screen shows three different timers:

- **Time in Motion:** accumulated time when the reported speed is equal to or greater than Min. Trip Speed.
- **Trip Time:** elapsed time since the trip timer started. Unlike Time in Motion, it includes time spent stationary or moving below the minimum speed threshold.
- **Fix Time:** elapsed time since the first valid GPS/GNSS fix was obtained.

Time in Motion is therefore normally shorter than Trip Time if the device spends part of the trip stopped or moving below the threshold. Fix Time describes how long it has been since the first valid fix, not how long the device has been moving.

### ➡️ Reset Trip

Choose **Reset Trip** in Settings to reset the trip tracking data, including:

- Time in Motion and Trip Time.
- The reference starting point used by the straight-line Distance screen.
- Accumulated Route Distance and average moving speed.
- Minimum and maximum speed.
- Minimum and maximum altitude.

After the reset, these values begin to be calculated again as new GPS/GNSS data is received. Reset Trip does not improve the receiver's signal or accuracy; it simply starts a fresh set of trip measurements.

### ➡️ Why it can help to reset after the first fix

The first fixes after a GPS/GNSS receiver starts may be less stable than later readings. While the receiver is acquiring satellites and refining its solution, position, speed, or altitude can fluctuate. These early readings may affect Time in Motion, Trip Time, the starting reference position for straight-line distance, Route Distance, average/minimum/maximum speed, and minimum/maximum altitude.

For more representative trip statistics, wait until the module's readings and position quality have settled, then open **Settings → Reset Trip** and reset the trip data. The firmware will calculate the trip values again from the data received after the reset. This is a practical recommendation, not a guarantee of a particular accuracy level.

### ➡️ Basic Settings: UART pins and baud rate

The **Basic Settings** screen displays the current RX GPIO, TX GPIO, and baud rate. It also provides a guided workflow to change them.

Basic Settings is shown automatically when the firmware starts and cannot find saved settings in NVS—for example, the first time the firmware runs or after the saved configuration has been cleared. Follow the on-screen instructions to choose the RX GPIO, TX GPIO, and baud rate.

Available GPIO choices:

`1`, `2`, `9`, `10`, `13`, `15`, `26`, `32`, `33`, `36`, `43`, `44`

Available baud rates:

`9600`, `19200`, `38400`, `57600`, `115200`

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/basic-settings-steps.webp" alt="Basic Settings configuration workflow" width="100%">
</p>

The same GPIO cannot be selected for both RX and TX. Not every GPIO listed in the menu is physically accessible on the StickS3. The list is intentionally broad so the firmware can be adapted to other compatible hardware, including other devices in the M5Stack family. A menu option does not mean that the pin is exposed or suitable on every device: check the target board's pinout before selecting it.

### ➡️ Screen Update

**Screen Update** controls how often the display is refreshed:

| Option | Behavior |
|---|---|
| **Automatic** | Refreshes the display when new GPS/GNSS data is received. |
| **1 second** | Refreshes at one-second intervals. |
| **5 seconds** | Refreshes every five seconds. |
| **15 seconds** | Refreshes every fifteen seconds. |
| **30 seconds** | Refreshes every thirty seconds. |
| **60 seconds** | Refreshes every sixty seconds. |

A fixed display refresh interval does not change the GPS/GNSS module's output rate or stop the firmware from processing incoming data. It only reduces how often the display is redrawn. Screen changes and interactions that require an immediate redraw can still refresh the screen immediately.

### ➡️ Screen Timeout

**Screen Timeout** turns the display off after a period without button interaction. Available options are **Disabled**, **10 seconds**, **30 seconds**, **60 seconds**, **300 seconds (5 minutes)**, and **600 seconds (10 minutes)**.

When the display turns off, GPS/GNSS data processing continues. Press **Button A** to turn the display back on. Disabling the timeout keeps the display on until it is turned off by another action or the device loses power. Using a timeout can reduce the time the display remains illuminated and may help save battery power.

### ➡️ Module Timeout and Communication Warning

The firmware checks whether new GPS/GNSS module data is arriving. **Module Timeout** controls when the **Communication Warning** appears if no new data is received.

Available options are **Disabled**, **5 seconds**, **15 seconds**, **30 seconds**, and **60 seconds**. The warning can help identify a disconnected module, incorrect UART wiring or GPIO selection, a mismatched baud rate, an interruption in communication, or a loss of power to the module.

The warning does not stop the firmware. Press **Button A** to dismiss it. Choosing **Disabled** hides the warning; it does not disable the firmware's handling of GPS/GNSS communication.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screen-warning.jpg" alt="Communication Warning screen" width="40%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screen-general-warning.jpg" alt="Communication Warning on General screen" width="40%">
</p>

### ➡️ Course is direction of travel, not a compass

**Course** is the direction in which the device is moving, calculated from GPS/GNSS navigation data. It is **not a magnetic compass** and does not measure which way the device is facing while stationary. A meaningful course generally requires movement; when there is no movement, the receiver may not provide a useful course value.

### ➡️ Measurement units and coordinate formats

The firmware converts values for display according to the selected setting. Changing a unit changes how a value is presented, not the GPS/GNSS receiver's underlying measurement.

| Setting | Available options | What changes |
|---|---|---|
| **Coord. Format** | Decimal, DMS, DM | How latitude and longitude are written. |
| **Speed Unit** | km/h, mph, knots, m/s | The displayed speed values, including speed statistics. |
| **Altitude Unit** | Meters, Feet | The displayed altitude values. |
| **Distance Unit** | Metric, Imperial | Distance units used by Distance and Route Stats. Metric uses meters/kilometers; Imperial uses feet/miles as appropriate for the display. |

Coordinate formats mean:

- **Decimal:** decimal degrees, such as `-23.550520`.
- **DMS:** degrees, minutes, and seconds.
- **DM:** degrees and decimal minutes.

Distance values are scaled for readability. In Metric mode, values can be shown in meters or kilometers; in Imperial mode, values use the corresponding imperial distance units. Speed can be displayed in kilometers per hour, miles per hour, knots, or meters per second. Altitude can be displayed in meters or feet.

### ➡️ Date, time format, and timezone

GPS/GNSS time is received in UTC. The firmware applies the selected **Timezone** offset when displaying local date and time. The available timezone list includes UTC offsets from **UTC−12:00 to UTC+14:00**, including several half-hour and quarter-hour offsets.

**Date Format** options:

- `YYYY-MM-DD`
- `MM/DD/YYYY`
- `DD/MM/YYYY`

**Time Format** options:

- **24-hour:** displays hours, minutes, and seconds.
- **12-hour:** displays AM or PM, but does not display seconds.

The firmware also handles date changes when the time adjusted to the selected timezone crosses midnight.

### ➡️ Position Quality

**Position Quality** is calculated by the firmware from HDOP; it is not a separate quality label sent by the GPS/GNSS module. HDOP describes satellite geometry and is an indicator of how favorable that geometry is for horizontal positioning. Lower HDOP generally indicates better geometry, but HDOP is not a direct measurement of position error in meters.

| HDOP value | Position Quality |
|---|---|
| `< 0.7` | Excellent |
| `0.7 to < 1.5` | Good |
| `1.5 to < 3.0` | Moderate |
| `3.0 to < 5.0` | Poor |
| `≥ 5.0` | Very Poor |

If a valid HDOP value has not yet been obtained, the firmware displays **No Fix**. Position Quality appears on the **Position**, **Speed**, and **Altitude** screens where applicable to the displayed layout.

### ➡️ NVS: saving configuration

The ESP32 includes **NVS (Non-Volatile Storage)**, a storage area that retains small configuration values when the device is restarted or powered off. GPS & GNSS Monitor uses NVS to save its settings, such as UART pins and baud rate, display colors and brightness, orientation, screen timeout and refresh interval, communication timeout, minimum trip speed, units, coordinate format, timezone, and date/time formats.

Settings are saved when changed, so ordinary customization does not require editing the source code. If the firmware cannot find a saved configuration, it opens the Basic Settings setup workflow at startup.

---

## 📡 GPS/GNSS Module Compatibility and Electrical Safety

### What makes a module compatible?

The firmware is designed for GPS/GNSS receivers that output standard **NMEA data over UART** and can be configured to use one of the supported baud rates. The firmware uses UART communication in `SERIAL_8N1` format: 8 data bits, no parity, and 1 stop bit.

A module is not guaranteed to work simply because it is described as a GPS or GNSS module. Before connecting one, check its documentation to confirm that it can output compatible NMEA data over UART, that its baud rate is supported, and that its voltage and logic levels are safe for the StickS3. Verify compatibility before testing an unfamiliar module.

### GPS versus GNSS

**GPS** is the satellite navigation system operated by the United States. **GNSS** is the broader term for Global Navigation Satellite Systems and can include GPS, Galileo, GLONASS, BeiDou, and other satellite constellations. A GNSS receiver that can use multiple constellations may be able to use more satellites in suitable conditions, but actual performance depends on the receiver, antenna, environment, and satellite visibility.

### Baud rates

The firmware offers these module baud rates:

`9600`, `19200`, `38400`, `57600`, `115200`

The selected rate must match the rate at which the module sends UART data. If the rates do not match, the StickS3 may receive unreadable data or no valid GPS/GNSS information. The baud rate selected in **Basic Settings** is the module communication rate; it is separate from the baud rate used by a computer when flashing the firmware.

### Voltage and UART logic-level safety

**Check voltage compatibility before connecting or powering the hardware.** The module's required supply voltage must be compatible with the voltage supplied by your chosen StickS3 connection (3.3 V or 5 V, depending on the connection and hardware specification). Do not assume that every module accepts either voltage.

Also check the module's UART signal levels, especially its **TX output**, which connects to the StickS3's RX input. The StickS3's GPIO logic is 3.3 V; the signal delivered to its GPIO must stay within the level supported by the StickS3. A module powered from 5 V may still output 5 V UART logic, which can damage the StickS3 if connected directly. Use a suitable level shifter or other properly designed interface if the module's TX signal is not 3.3 V-compatible.

Connect **GND to GND**, cross the UART signals correctly, and verify the module pinout before powering on:

- Module **TX → StickS3 RX**
- Module **RX ← StickS3 TX** (if the module requires receiving commands)
- Module **GND ↔ StickS3 GND**

The module's supply voltage and UART logic voltage are separate specifications; verify both in the module documentation.

### Accuracy and receiver quality

Different receivers, antennas, antenna placement, satellite visibility, signal strength, buildings, trees, reflections, atmospheric conditions, and other environmental factors can change fix acquisition time and the accuracy of position, speed, and altitude. The firmware displays and processes the information received from the receiver; it cannot create accuracy that the receiver's data does not provide.

For more reliable results, use a good-quality module and antenna, allow the receiver time to acquire a stable fix, and test in an environment with a clear view of the sky when possible.

### Recommended module: REYAX RYS352A

I used the **REYAX RYS352A** during development and testing and strongly recommend it based on my experience. In my tests, it provided excellent positioning quality and precision and acquired satellites quickly. Results can still vary with antenna placement, environment, and satellite conditions.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/RYS352A.png" alt="REYAX RYS352A GNSS module" width="70%">
</p>

- **Official product page:** [reyax.com/product/GPS-GNSS/RYS352A](https://reyax.com/product/GPS-GNSS/RYS352A)
- **DigiKey:** [digikey.com/en/products/detail/reyax/RYS352A/22206992](https://www.digikey.com/en/products/detail/reyax/RYS352A/22206992)
- **eBay:** [ebay.com/itm/187031730034](https://www.ebay.com/itm/187031730034)
- **Amazon:** [amazon.com/dp/B0CM5JTJL7](https://www.amazon.com/dp/B0CM5JTJL7?lv=shuf&language=zh_TW&channelId=500&plpRedirect=mhFallback)

---

## 🔌 Wiring Diagram and UART Configuration

UART uses crossed signal connections: the module's TX sends data to the firmware's RX, and the firmware's TX connects to the module's RX if communication in that direction is needed.

### Recommended StickS3 connections

For the M5StickS3, these are the recommended pin assignments described for the respective connector:

#### Hat2-Bus
| StickS3 connection | Configure as | Connect to module |
|---|---|---|
| **Hat2-Bus G44** | RX | Module TX |
| **Hat2-Bus G43** | TX | Module RX |

#### HY2.0-4P
| StickS3 connection | Configure as | Connect to module |
|---|---|---|
| **HY2.0-4P G10** | RX | Module TX |
| **HY2.0-4P G9** | TX | Module RX |

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/wiring-diagram.webp" alt="GPS/GNSS UART wiring diagram" width="100%">
</p>

Always confirm the pinout for your specific module and StickS3 connection before wiring. If the module only sends NMEA data and does not need to receive commands, its RX connection may not be needed for basic monitoring.

### Available GPIOs and baud rates

RX and TX can each be selected from the following firmware options:

`1`, `2`, `9`, `10`, `13`, `15`, `26`, `32`, `33`, `36`, `43`, `44`

Available baud rates:

`9600`, `19200`, `38400`, `57600`, `115200`

These settings are available on the **Basic Settings** screen. Press Button A to enter the configuration workflow, use Button B to cycle through the available choices, and press Button A to confirm each one. The same GPIO cannot be used for RX and TX.

The GPIO list includes pins intended to make the firmware adaptable to other hardware, including other M5Stack devices. Some listed pins may not be physically exposed on the M5StickS3, or may not be suitable for a particular connector or peripheral. Selecting a GPIO in the menu does not make it physically accessible. Check your board's pinout and confirm that the selected pins are available before connecting the module.

---

## 💻 Installation

There are three ways to install GPS & GNSS Monitor: through M5Burner, by flashing the `.bin` firmware file, or by compiling the source code with Arduino IDE.

### 1. Install using M5Burner

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-gps-gnss-monitor.webp" alt="GPS & GNSS Monitor in M5Burner" width="70%">
</p>

M5Burner is a convenient option because it lets you install the firmware without manually downloading a `.bin` file.

#### You can access the Firmware page in M5Burner in one of the following ways:

- Visit [burner.m5stack.com](https://burner.m5stack.com/), select **StickS3**, and search for `GPS & GNSS Monitor`.
- Open the firmware directly through this link: [https://burner.m5stack.com/A8XVKK](https://burner.m5stack.com/A8XVKK).

> **💡 Recommendation:** For the best experience when using M5Burner, I recommend using **Google Chrome**.

#### Put the StickS3 into programming mode

1. Press and hold the StickS3 **Power** button.
2. Keep holding it until the indicator LED starts blinking.
3. The device is now in download/programming mode.

#### Flash the firmware

1. Connect the StickS3 to your computer with a USB cable.
2. Keep the StickS3 in programming mode.
3. On the M5Burner page, select the correct connected device/serial port when prompted.
4. Follow M5Burner's on-screen instructions to upload the firmware.
5. Wait for the process to complete.
6. Restart the StickS3 if it does not restart automatically.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-1.webp" alt="M5Burner installation step 1" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-2.webp" alt="M5Burner installation step 2" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-3.webp" alt="M5Burner installation step 3" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-4.webp" alt="M5Burner installation step 4" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-5.webp" alt="M5Burner installation step 5" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-6.webp" alt="M5Burner installation step 6" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-7.webp" alt="M5Burner installation step 7" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-8.webp" alt="M5Burner installation step 8" width="70%">
</p>

### 2. Install using the `.bin` firmware file

This method uses [Espressif ESP Tool JS](https://espressif.github.io/esptool-js/), a browser-based flashing tool. 

> Google Chrome is recommended because it generally offers good Web Serial support.

#### Put the StickS3 into programming mode

1. Press and hold the **Power** button.
2. Keep holding it until the indicator LED starts blinking.
3. Connect the StickS3 to your computer using USB.

#### Flash the `.bin` file

1. Download `GPS_GNSS_Monitor.bin` from this repository's.
2. Open ESP Tool JS: [espressif.github.io/esptool-js/](https://espressif.github.io/esptool-js/).
3. Set the flashing baud rate to `115200`.
4. Connect to the StickS3 and select its COM/serial port.
5. Select `GPS_GNSS_Monitor.bin` as the firmware file.
6. Set **Flash Address** to `0x0`.
7. Set **Flash Mode** to `dio`.
8. Set **Flash Frequency** to `80m`.
9. Set **Flash Size** to `8MB`.
10. Start flashing and wait until the tool reports completion.
11. Restart the StickS3.

#### Use these ESP Tool JS settings:

| ESP Tool JS setting | Value |
|---|---|
| Baud rate | `115200` |
| Flash Address | `0x0` |
| Flash Mode | `dio` |
| Flash Frequency | `80m` |
| Flash Size | `8MB` |

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/esptool-1.png" alt="ESP Tool JS setup" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/esptool-2.png" alt="ESP Tool JS firmware selection" width="70%">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/esptool-3.png" alt="ESP Tool JS flashing settings" width="70%">
</p>

### 3. Install and upload using Arduino IDE

Use this method if you want to compile the source code yourself or modify the firmware.

#### Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- M5Stack board support for the **M5StickS3**
- `M5Unified` library
- `M5GFX` library
- `TinyGPSPlus` library

#### Installation and upload

1. Install Arduino IDE.
2. Download or clone this repository to your computer.
3. Open `GPS_GNSS_Monitor.ino` in Arduino IDE.
4. Install the required libraries through Arduino IDE's Library Manager if they are not already installed.
5. Install the appropriate M5Stack board support and select **M5StickS3** as the board.
6. Connect the StickS3 to your computer with USB.
7. Select the serial port corresponding to the StickS3.
8. Compile the sketch to check for errors.
9. Upload the firmware to the device.
10. Restart the StickS3 if necessary.
11. Connect the GPS/GNSS module and configure the module's baud rate and UART GPIOs in **Basic Settings** so they match the module and wiring.

See the official [M5StickS3 Arduino programming documentation](https://docs.m5stack.com/en/arduino/m5sticks3/program) for board setup and upload guidance.

---

## 🦈 Bruce Firmware with GPS/GNSS Information

I also modified the **Bruce** firmware to add a dedicated GPS Info screen that displays additional data received from a connected GPS/GNSS module. This modified version also supports changing measurement units and saving GPS/GNSS information to **LittleFS** or a **memory card**, where supported by the hardware and firmware setup.

If you are interested in this alternative firmware, visit the repository:

**[github.com/viniciushnf/StickS3-GPS-GNSS](https://github.com/viniciushnf/StickS3-GPS-GNSS)**

---

## 👋 Questions, Suggestions, and Project Sharing

If you have questions, suggestions, or encounter a problem, feel free to contact me on Instagram. If you build this project and want to share your result, I would be very happy to see it!

- **Instagram:** [@viniciushnf](https://www.instagram.com/viniciushnf/)

Feedback, project photos, and suggestions are welcome.

If you find this project useful, consider giving the repository a ⭐ on GitHub. Enjoy experimenting with GPS and GNSS!

---

## 📄 License

This project is released under the **MIT License**.
