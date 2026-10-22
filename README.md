<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/cover.webp" alt="Cover" width="100%">
</p>

# GPS & GNSS Monitor for M5StickS3

GPS & GNSS Monitor transforms the M5Stack StickS3 into a compact, portable GPS/GNSS monitoring device.

Connect a compatible GPS or GNSS module through UART to display positioning, satellite reception, navigation, date and time, and trip statistics directly on the device.

The firmware provides configurable UART RX and TX GPIOs, baud rate, display appearance and orientation, screen update intervals, communication timeout, measurement units, and minimum trip speed. Configuration settings are saved in non-volatile storage, allowing users to customize the device without modifying the source code.

It is designed for portable navigation, GPS/GNSS module testing, field monitoring, positioning experiments, and exploring GNSS data.

---

## 🚀 Features

- 📡 **Real-Time GPS/GNSS Monitoring** — Receive and display navigation data from a compatible GPS/GNSS module over UART.
- 📍 **Position** — Latitude and longitude with configurable coordinate formats.
- 🛰️ **Satellite & Position Quality** — Satellite count, HDOP, and a qualitative indication of positioning quality.
- 🚗 **Speed** — Current, average moving, minimum, and maximum speed, with selectable measurement units.
- ⛰️ **Altitude** — Current, minimum, and maximum altitude, with selectable units.
- 📏 **Distance Tracking** — Straight-line distance from the trip starting position and accumulated route distance.
- 🛣️ **Route Statistics** — Track route distance, average moving speed, minimum and maximum speed, minimum and maximum altitude, and time in motion.
- ⏱️ **Session Information** — Monitor time in motion, trip time, and time since the first valid GPS/GNSS fix.
- 🧭 **Course** — Display the direction of travel using GPS/GNSS course data.
- 📅 **Date & Time** — Display GPS/GNSS date and time with configurable formats and timezone.
- 🔌 **Configurable UART** — Select RX and TX GPIOs and the module baud rate directly on the device.
- 🎨 **Theme & Background Colors** — Customize the interface using 14 available colors.
- 💡 **Display Brightness** — Adjust screen brightness.
- 🔄 **Screen Orientation** — Flip horizontal or vertical screens by 180 degrees or apply an additional 90-degree rotation.
- 🔋 **Screen Timeout** — Configure when the display turns off after inactivity, or disable the timeout.
- 🔃 **Screen Update Interval** — Choose automatic updates or refresh intervals of 1, 5, 15, 30, or 60 seconds.
- ⚠️ **Configurable Communication Warning** — Set the module timeout to 5, 15, 30, or 60 seconds, or disable the warning.
- 🚦 **Minimum Trip Speed** — Configure the minimum speed used to record route distance and time in motion, from 0 to 10 km/h in 0.5 km/h increments.
- 📐 **Measurement & Format Settings** — Configure coordinate format, speed units, altitude units, distance units, date format, time format, and timezone.
- 💾 **Persistent Configuration** — Settings are stored in non-volatile storage (NVS) and retained after restarting the device.
- 🔄 **Trip Reset** — Reset trip data and route statistics.
- 🧭 **Configurable Display Navigation** — Navigate between screens and adjust settings using the device buttons.

---

# 🖥️ Screens

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens.gif" alt="Screens" width="70%">
</p>

### 📡 General

Provides an overview of the current GPS/GNSS status, including satellite information and position quality.

The General section includes four screen layouts: one landscape layout and three portrait-oriented sub-screens.

Press **Button A** to cycle through the available General layouts.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-general.gif" alt="Screens General" width="30%">
</p>

### 📍 Position

Displays the current geographic coordinates.

Depending on the selected coordinate format, the position can be displayed using different coordinate representations.

### 🚗 Speed

Displays the current speed received from the GPS/GNSS module.

The speed unit can be configured in the settings.

### ⛰️ Altitude

Displays the current altitude reported by the GPS/GNSS module.

The altitude unit can be changed between meters and feet.

### 📏 Distance

Displays the distance between the **trip starting point** and the current position.

This is a straight-line distance and should not be confused with the accumulated route distance.

### 🛣️ Route Stats

Provides statistics related to the movement recorded during the current trip.

The Route Stats section includes three screen layouts: one landscape layout and two portrait-oriented sub-screens.

One of the portrait layouts includes a graphical indicator that compares current speed with the recorded average, minimum, and maximum speeds.

Press **Button A** to cycle through the available Route Stats layouts.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-route-stats.gif" alt="Screens General" width="30%">
</p>

The firmware can calculate:
* Total route distance
* Average moving speed
* Minimum speed
* Maximum speed
* Minimum altitude
* Maximum altitude
* Moving time

Route distance is accumulated from consecutive GPS/GNSS positions while the reported speed meets or exceeds the configured minimum trip speed. This threshold helps reduce unwanted distance accumulation caused by GPS/GNSS position drift while the device is stationary.

The minimum trip speed can be configured through the `Min. Trip Speed` setting.

### 🧭 Course

Displays the current direction of travel and compass direction calculated from the GNSS data.

### 🕐 Time

Displays the current date and time received from the GPS/GNSS module.

GNSS time is received in UTC and converted according to the timezone configured in the firmware.

The firmware also handles date changes when the converted time crosses midnight.

### 📈 Session Info

Displays three time-related values:

- **Time in Motion** — Accumulated time during which the reported speed meets or exceeds the configured minimum trip speed.
- **Trip Time** — Elapsed time since the trip started.
- **Fix Time** — Elapsed time since the first valid GPS/GNSS fix was obtained.

Time in motion is tracked separately from trip time. Therefore, time spent stationary or moving below the configured minimum trip speed is not added to the time-in-motion counter.

### ℹ️ About

The About screen provides firmware information, including the firmware name, version, selected RX and TX GPIOs, and module baud rate.

Press **Button A** to switch to a second About view containing the developer information and a link to the project repository on GitHub. Press Button A again to return to the first view.

### ⚙️ Settings

The Settings menu provides access to the firmware's configurable options, including display appearance, screen orientation, GPS/GNSS communication, measurement units, date and time formats, update intervals, communication timeout, minimum trip speed, and trip reset.

Configuration values are saved to NVS so that the selected preferences are retained after restarting the device.

## Configuration Screens

The Settings menu provides the following configuration options:

| Screen | Description |
|---|---|
| `Theme Color` | Select the color used for interface elements and text. |
| `Background Color` | Select the display background color. |
| `Brightness` | Adjust the display brightness. |
| `Screen Timeout` | Configure the inactivity period before the display turns off, or disable the timeout. |
| `Screen Flip H` | Rotate horizontal screens by 180 degrees when enabled. |
| `Screen Flip V` | Rotate vertical screens by 180 degrees when enabled. |
| `Screen Rotate` | Apply an additional 90-degree rotation to the display. |
| `Coord. Format` | Select the coordinate display format. |
| `Speed Unit` | Select the speed measurement unit. |
| `Min. Trip Speed` | Set the minimum speed used to record route distance and time in motion. |
| `Altitude Unit` | Select the altitude measurement unit. |
| `Distance Unit` | Select the distance measurement unit. |
| `Timezone` | Configure the timezone used to display local date and time. |
| `Date Format` | Select the date display format. |
| `Time Format` | Select the time display format. |
| `Screen Update` | Select automatic screen updates or a fixed refresh interval. |
| `Module Timeout` | Configure when the GPS/GNSS communication warning appears, or disable the warning. |
| `Basic Settings` | View and configure the UART RX GPIO, TX GPIO, and baud rate. |
| `Reset Trip` | Reset trip data and route statistics. |
| `Exit` | Leave the Settings menu and return to the General screen.


<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-config.gif" alt="GPS & GNSS Monitor Screens" width="70%">
</p>

---

# 🎛️ Button Controls

The M5StickS3 buttons are used to navigate between screens, switch views, and configure the firmware.

### Button B

- **Short press:** Move to the next screen.
- **Long press:** Move to the previous screen.

### Button A

The function of Button A depends on the current screen. It can be used to:

- Change the displayed format or view.
- Enter the Settings menu and change configuration values.
- Switch between the four General views.
- Switch between the three Route Stats views.
- Switch between the two About views.
- Open the UART configuration workflow from `Basic Settings`.
- Confirm configuration choices.
- Turn the display back on after a screen timeout.
- Dismiss the GPS/GNSS communication warning.

### Inside the Settings Menu

- **Button B — Short press:** Move to the next setting.
- **Button B — Long press:** Move to the previous setting.
- **Button A:** Change, select, or confirm the current setting, depending on the screen.

The `Basic Settings` configuration workflow also uses Button B to cycle through the available options and Button A to confirm each selection.

---

# 🔋 Display Timeout

The firmware includes a **display timeout** function designed to help save battery power.

If there is no user interaction for the configured period of time, the display automatically turns off.

GPS/GNSS processing continues normally while the display is turned off.

To turn the display back on:

> **Press Button A.**

The display timeout can be configured through the Settings menu.

You can:

* ⏱️ Change the amount of time before the display turns off.
* 🔴 Disable the display timeout completely.

This allows the device to continue monitoring GPS/GNSS data without keeping the screen continuously illuminated.

---

# ⚠️ GPS/GNSS Communication Warning

The firmware monitors incoming GPS/GNSS data and can display a `Communication Warning` when no new module data is received within the configured timeout.

This warning can help identify issues such as:

- 🔌 A disconnected GPS/GNSS module.
- 🔧 Incorrect UART wiring or GPIO selection.
- ⚙️ An incorrect baud rate.
- 📡 Temporary communication interruptions.
- 🔋 A loss of power to the GPS/GNSS module.

### Module Timeout Settings

The warning timeout can be configured through the `Module Timeout` screen in the Settings menu.

Available options:

| Setting | Behavior |
|---|---|
| `Disabled` | Never display the communication warning, regardless of how long the module remains silent. |
| `5 seconds` | Display the warning after 5 seconds without new module data. |
| `15 seconds` | Display the warning after 15 seconds without new module data. |
| `30 seconds` | Display the warning after 30 seconds without new module data. |
| `60 seconds` | Display the warning after 60 seconds without new module data. |

The warning does not stop the firmware. When it appears, press **Button A** to dismiss it and continue using the device.

Disabling the warning does not disable GPS/GNSS communication monitoring itself; it only prevents this warning from being displayed.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screen-warning.jpg" alt="GPS & GNSS Monitor Screens" width="50%">
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screen-general-warning.jpg" alt="GPS & GNSS Monitor Screens" width="50%">
</p>

---

# 🔃 Screen Update Interval

The firmware allows you to control how frequently the display is refreshed through the `Screen Update` setting.

Available options:

| Setting | Behavior |
|---|---|
| `Automatic` | Refresh the display when new GPS/GNSS data is received. |
| `1 second` | Refresh the display at 1-second intervals. |
| `5 seconds` | Refresh the display at 5-second intervals. |
| `15 seconds` | Refresh the display at 15-second intervals. |
| `30 seconds` | Refresh the display at 30-second intervals. |
| `60 seconds` | Refresh the display at 60-second intervals. |

In automatic mode, the display updates as new GPS/GNSS information is received.

When a fixed interval is selected, GPS/GNSS data processing continues independently of the display refresh interval. The screen can therefore show updates less frequently without requiring the module to change its output rate.

**Important:** Button interactions that require a screen change or an immediate redraw are not restricted by the selected refresh interval. Navigating between screens and interacting with settings can trigger a refresh immediately.

This feature is useful when working with GPS/GNSS modules that send data frequently and you want to reduce unnecessary screen redraws.

---

# 🎨 Display Appearance & Orientation

The firmware provides several options for customizing the display's appearance and orientation.

### Theme Color

`Theme Color` controls the color used for interface elements, text, and visual indicators.

### Background Color

`Background Color` controls the display's background color.

The firmware provides 14 available colors for both settings:

`WHITE`, `BLACK`, `RED`, `GREEN`, `BLUE`, `YELLOW`, `CYAN`, `MAGENTA`, `ORANGE`, `PURPLE`, `PINK`, `NAVY`, `BROWN`, `GRAY`

The theme color and background color cannot be set to the same color, preventing the interface from becoming unreadable.

### Screen Orientation

The firmware provides three orientation settings:

- **Screen Flip H:** Rotates horizontal screens by 180 degrees when enabled.
- **Screen Flip V:** Rotates vertical screens by 180 degrees when enabled.
- **Screen Rotate:** Applies an additional 90-degree rotation through the display rotation configuration.

These options can help correct display orientation issues and make it easier to adapt the firmware to other hardware with different display orientations.

### Screen Timeout

`Screen Timeout` controls how long the device can remain without user interaction before the display turns off.

The timeout can be configured for 10, 30, 60, 300, or 600 seconds, or disabled completely.

When the display turns off, GPS/GNSS processing continues. Press **Button A** to turn the display back on.

---

# 🚗 Minimum Trip Speed & Time in Motion

The `Min. Trip Speed` setting controls the minimum GPS/GNSS-reported speed required for the firmware to record route distance and accumulate time in motion.

### Minimum Trip Speed

Available values range from `0` to `10 km/h`, in increments of `0.5 km/h`.

The selected value is displayed using the configured speed measurement unit.

A higher threshold can help reduce unwanted route-distance accumulation caused by GPS/GNSS position drift while the device is stationary. A lower threshold allows slower movement to be included in the trip statistics.

### Time in Motion

Time in motion is accumulated only while the reported speed is equal to or greater than the configured minimum trip speed.

This value is different from trip time:

- **Trip Time:** Elapsed time since the trip started.
- **Time in Motion:** Accumulated time during which the speed meets the minimum trip speed threshold.
- **Fix Time:** Elapsed time since the first valid GPS/GNSS fix was obtained.

Time in motion is available on the `Session Info` screen and in the route statistics.

The minimum speed threshold affects route distance recording and time-in-motion accumulation. It does not change the GPS/GNSS module's reported speed.

---

# 📡 GPS/GNSS Compatibility

GPS & GNSS Monitor is designed to work with GPS/GNSS modules that provide standard **NMEA data over UART**.

The firmware communicates with the module using:

```text
SERIAL_8N1
```

This means:

* 8 data bits
* No parity
* 1 stop bit

The firmware supports several common UART baud rates, including:

* 9600
* 19200
* 38400
* 57600
* 115200

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/baudrate.jpg" alt="GPS & GNSS Monitor Screens" width="50%">
</p>

Different GPS/GNSS modules can provide different levels of:

* 📍 Position accuracy
* 🛰️ Satellite acquisition performance
* ⚡ Fix acquisition speed
* 📡 Signal sensitivity
* 🔄 Update rate

Therefore, the overall performance of the system depends not only on the firmware, but also on the GPS/GNSS module, antenna, satellite visibility, environment, and signal conditions.

---

## 🔌 UART Configuration

The `Basic Settings` screen displays the currently selected UART RX GPIO, TX GPIO, and baud rate.

Press **Button A** on this screen to configure the communication settings. The firmware guides you through selecting the RX GPIO, TX GPIO, and baud rate.

During configuration:

- **Button B:** Cycle through the available options.
- **Button A:** Confirm the selected option and continue.

The available GPIO options for RX and TX are:

`1, 2, 9, 10, 13, 15, 26, 32, 33, 36, 43, 44`

The firmware does not allow the same GPIO to be selected for both RX and TX.

The available module baud rates are:

`9600, 19200, 38400, 57600, 115200`

Choose the baud rate supported by your GPS/GNSS module. RX and TX must also be configured to match the actual wiring.

### UART Signal Direction

UART connections must be crossed:

- **GPS/GNSS module TX → Firmware RX GPIO**
- **GPS/GNSS module RX ← Firmware TX GPIO**

The module's TX signal sends data to the firmware. The firmware's TX signal is used to transmit data to the module's RX input.

### Recommended M5StickS3 Connections

The recommended GPIO configuration depends on the connection interface.

#### Hat2-Bus

- Configure **G44 as RX**.
- Configure **G43 as TX**.
- Connect the GPS/GNSS module's TX to **G44**.
- Connect the GPS/GNSS module's RX to **G43**, following the recommended Hat2-Bus wiring.

#### HY2.0-4P

- Configure **G10 as RX**.
- Configure **G9 as TX**.
- Connect the GPS/GNSS module's TX to **G10**.
- Connect the GPS/GNSS module's RX to **G9**.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/wiring-diagram.png" alt="GPS & GNSS Monitor Screens" width="100%">
</p>

Always verify the wiring and the pinout of your hardware before powering on the system.

### GPIO Availability and Hardware Adaptation

Not every GPIO available in the firmware's selection menu is physically accessible on the M5StickS3.

The GPIO list is intentionally broader to make the firmware easier to adapt to other devices, including the M5 Cardputer and other devices in the M5Stick family.

The available GPIOs, their physical accessibility, and their suitability for UART communication depend on the target device. Before selecting a GPIO, verify that it exists on your hardware and can be used for the required UART signal.

Selecting a GPIO in the firmware does not make that pin physically available on the device.

### Electrical Compatibility

Before connecting a GPS/GNSS module, verify its supply voltage and UART logic-level requirements. Connect the module's GND to the device's GND, and use a compatible power supply.

---

# 🧰 Hardware Assembly

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/pcb-1.jpg" alt="PCB" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/photo-6.jpg" alt="Photo" width="70%">
</p>

For my hardware setup, I soldered the necessary pins to a small PCB and wired the GPS/GNSS module and the StickS3 together.

This creates a compact assembly where the GPS/GNSS module and the StickS3 remain firmly attached to each other.

This type of assembly is especially useful for:

* Portable GPS/GNSS testing
* Field experiments
* Navigation
* Development and prototyping
* Long-term monitoring

The exact mechanical assembly can be adapted according to the GPS/GNSS module being used.

---

# ⭐ Recommended GPS/GNSS Module

I developed and tested the firmware using the **REYAX RYS352A**, supplied by REYAX.

I was very satisfied with the module during my tests, particularly with its **quality, positioning precision, and fast FIX acquisition**.

For this reason, I strongly recommend the RYS352A for anyone looking for a high-quality GPS/GNSS module for this project.

The RYS352A supports multiple GNSS systems and provides NMEA output over UART, with navigation updates of up to 10 Hz.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/RYS352A.png" alt="GPS & GNSS Monitor Screens" width="70%">
</p>

### 🛰️ REYAX RYS352A

Official product page:

https://reyax.com/product/GPS-GNSS/RYS352A

Purchase options:

* **DigiKey:** https://www.digikey.com/en/products/detail/reyax/RYS352A/22206992
* **eBay:** https://www.ebay.com/itm/187031730034
* **Amazon:** https://www.amazon.com/dp/B0CM5JTJL7?lv=shuf&language=zh_TW&channelId=500&plpRedirect=mhFallback

---

# 🎯 Position Accuracy

The actual positioning accuracy depends on several factors, including:

* GPS/GNSS module
* Antenna quality
* Number of satellites in view
* Satellite geometry
* Signal strength
* Indoor or outdoor environment
* Obstructions
* Multipath effects
* Atmospheric conditions

Different modules can therefore produce different results even when running the same firmware.

The REYAX RYS352A used during development provided very good results in my tests, including good precision and fast FIX acquisition.

The firmware itself does not artificially improve the positioning accuracy provided by the GNSS receiver. It displays and processes the information received from the module.

---

# 📊 HDOP and Position Quality

The firmware also uses **HDOP (Horizontal Dilution of Precision)** as an indicator related to horizontal positioning quality.

HDOP is affected by satellite geometry and can help indicate how favorable the current satellite configuration is for determining the horizontal position.

A lower HDOP generally indicates better satellite geometry, while a higher value indicates less favorable geometry.

However, HDOP should not be interpreted as a direct measurement of position accuracy in meters.

The firmware displays HDOP and converts the value into a qualitative description.

The current implementation uses the following ranges:

|          HDOP | Firmware indication     |
| ------------- | ----------------------- |
|       `< 0.7` | **Excellent**           |
| `0.7 – < 1.5` | **Good**                |
| `1.5 – < 3.0` | **Moderate**            |
| `3.0 – < 5.0` | **Poor**                |
|       `≥ 5.0` | **Very Poor**           |

When no valid HDOP value has been obtained yet, the firmware displays: No Fix.

---

# 🛣️ Distance and Route Statistics

The firmware uses two different concepts for distance.

## 📏 Distance

The **Distance** screen calculates the straight-line distance between:

```text
Trip starting position
        ↓
Current position
```

This means it represents the displacement from the starting point rather than the actual path traveled.

For example, if you travel around a large area and eventually return close to your starting point, the Distance value may become small even though you have traveled a much longer route.

---

## 🛣️ Route Stats

**Route Stats** calculates accumulated route distance.

The firmware calculates the distance between consecutive GNSS positions and adds these values together while the device is considered to be moving.

Conceptually:

```text
Point 1 → Point 2
Point 2 → Point 3
Point 3 → Point 4
Point 4 → Point 5
        ↓
Accumulated Route Distance
```

To prevent small GNSS position variations while stationary from being interpreted as movement, the firmware only accumulates route distance when the GNSS-reported speed is above the configured internal minimum moving speed.

### ⚠️ Route Stats is an estimate

Route Stats should be considered an **estimate of the distance traveled**, not a precision surveying measurement.

GNSS positioning naturally contains small variations. Because of this, the calculated route distance can differ from the actual physical distance traveled.

The minimum moving-speed threshold helps reduce stationary GNSS drift, but it cannot completely eliminate measurement errors.

The minimum moving-speed value is an **internal firmware parameter** and cannot currently be changed through the Settings menu.

---

# 💾 Persistent Settings

GPS & GNSS Monitor stores its configuration in the ESP32's non-volatile storage (NVS).

The available settings can be edited directly through the device's Settings menu, including:

- UART RX and TX GPIOs and module baud rate.
- Theme and background colors.
- Display brightness, timeout, and orientation.
- Screen update interval and module timeout.
- Minimum trip speed.
- Coordinate format and measurement units.
- Timezone, date format, and time format.

Settings are saved automatically when changed and are normally retained after restarting or powering off the device.

This allows users to customize the firmware without modifying the source code for ordinary configuration changes.

---

# 💻 Installation

There are three ways to install GPS & GNSS Monitor.

## 1. 📦 Install the `.bin` Firmware

Both firmware options can be installed on the StickS3 using the same flashing procedure with **ESP Tool JS**.

Espressif ESP Tool JS: [https://espressif.github.io/esptool-js/](https://espressif.github.io/esptool-js/)

> **Recommendation:** Use **Google Chrome** when accessing ESP Tool JS, as it generally provides the best Web Serial support for this type of browser-based flashing tool.

### Entering Programming Mode

Before flashing the firmware, the StickS3 must be placed into **download/programming mode**.

1. Press and hold the **Power button**.
2. Keep the button pressed until the indicator LED starts flashing.
3. The StickS3 is now ready to be programmed.

### Flashing Procedure

1. Put the StickS3 into programming mode.
2. Connect the StickS3 to your computer using USB.
3. Download `GPS_GNSS_Monitor.bin` from this repository.
4. Open [ESP Tool JS](https://espressif.github.io/esptool-js/).
5. Set the baud rate to `115200`.
6. Select the StickS3's **COM port**.
7. Select the `.bin` file in ESP Tool JS.
8. Set **Flash Address** to `0x0`.
9. Set **Flash Mode** to `dio`.
10. Set **Flash Frequency** to `80m`.
11. Set **Flash Size** to `8MB`.
12. Start the flashing process.
13. Wait for the process to finish.
14. Restart the StickS3.

### ESP Tool JS screenshots

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/esptool-1.png" alt="ESPTool 1" width="70%">
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/esptool-2.png" alt="ESPTool 2" width="70%">
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/esptool-3.png" alt="ESPTool 3" width="70%">
</p>

### Required Settings

When flashing either firmware, use the following settings:

| ESP Tool JS Setting | Value    |
| ------------------- | -------- |
| **Baudrate**        | `115200` |
| **Flash Address**   | `0x0`    |
| **Flash Mode**      | `dio`    |
| **Flash Frequency** | `80m`    |
| **Flash Size**      | `8MB`    |

---

## 2. 🛠️ Install Using Arduino IDE

The complete source code is also provided in the repository.

Main source file:

```text
GPS_GNSS_Monitor.ino
```

### Requirements

* Arduino IDE
* M5Stack StickS3 board support
* M5Unified library
* M5GFX library
* TinyGPSPlus library

### Installation

1. Install Arduino IDE.
2. Open the `GPS_GNSS_Monitor.ino` file.
3. Install the required libraries.
4. Select the **M5StickS3** board.
5. Connect the StickS3 using USB.
6. Select the correct serial port.
7. Compile and upload the firmware.
8. Restart the device.
9. Connect the GPS/GNSS module.
10. Configure the GNSS baud rate if necessary.

Official M5Stack Arduino documentation: [docs.m5stack.com/en/arduino/m5sticks3/program](https://docs.m5stack.com/en/arduino/m5sticks3/program)

---

## 3. 🔥 Install Using M5Burner

The **GPS & GNSS Monitor** firmware is also available directly through **M5Burner**, making it easy to install the firmware on your StickS3 without manually downloading the firmware file.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-gps-gnss-monitor.webp" alt="GPS & GNSS Monitor Screens" width="70%">
</p>

### Access the Firmware

You can access the firmware in either of the following ways:

* Visit [M5Burner](https://burner.m5stack.com/), select **StickS3**, and search for `GPS & GNSS Monitor`.
* Open the firmware directly through [this link](https://burner.m5stack.com/A8XVKK).

> **💡 Recommendation:** For the best experience when using M5Burner, I recommend using **Google Chrome**.

### Installing the Firmware with M5Burner

1. Put the StickS3 into **programming mode** by pressing and holding the **Power** button until the indicator light starts blinking.
2. Connect the **StickS3** to your computer using a **USB cable**.
3. Open [M5Burner](https://burner.m5stack.com/) using **Google Chrome**.
4. Select **StickS3** and search for **GPS & GNSS Monitor**, or open the [direct firmware page](https://burner.m5stack.com/85KU9L).
5. Follow the M5Burner instructions to select the StickS3 and upload the firmware.
6. Wait for the installation to complete. Once finished, the StickS3 will be ready to use with the **GPS & GNSS Monitor** firmware.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-1.webp" alt="Step 1" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-2.webp" alt="Step 2" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-3.webp" alt="Step 3" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-4.webp" alt="Step 4" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-5.webp" alt="Step 5" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-6.webp" alt="Step 6" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-7.webp" alt="Step 7" width="70%">
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/m5burner-installation-steps/m5burner-installation-steps-8.webp" alt="Step 8" width="70%">
</p>

---

# 🦈 Bruce Firmware with GPS Info

I also made available a **modified version of the Bruce firmware** that adds a dedicated: GPS Info screen.

This screen provides several pieces of information received from the connected GPS/GNSS module and can be useful for users who want to inspect GNSS information while using Bruce.

If you are interested in installing this modified Bruce firmware, check the repository:
[github.com/viniciushnf/StickS3-GPS-GNSS](https://github.com/viniciushnf/StickS3-GPS-GNSS)

---

# 👋 Get in touch

If you build this project, I'd love to see the result! 

If you have any questions, suggestions, or run into any issues, feel free to contact me on Instagram. 

I'm always happy to help, receive feedback, and see what the community creates. 

I'm also open to collaborations and partnership opportunities related to electronics, embedded systems, and open-source projects.

* Instagram: **@viniciushnf**
* [instagram.com/viniciushnf](https://www.instagram.com/viniciushnf/)

---

## ⭐ If You Find This Project Useful

If this project helps you turn your StickS3 into a practical GPS/GNSS device, consider giving the repository a ⭐ on GitHub.

Enjoy experimenting with GPS and GNSS!

---

# 🙏 Acknowledgments

Special thanks to **REYAX** for providing the **RYS352A GNSS module** used during the development and testing of this project.

Thank you for supporting the project and for providing a module that performed very well during testing.

---

# 📄 License

This project is released under the **MIT License**.