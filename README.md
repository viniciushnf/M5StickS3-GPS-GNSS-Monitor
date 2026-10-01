<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/cover.webp" alt="Cover" width="100%">
</p>

# GPS & GNSS Monitor for M5StickS3

GPS & GNSS Monitor transforms your **M5Stack StickS3** into a compact, real-time GPS/GNSS monitoring device.

Simply connect a compatible GPS or GNSS module through UART and the firmware can display your current position, satellite information, speed, altitude, distance traveled, course, date, time, and route statistics directly on the device.

It is designed for **portable navigation, GPS/GNSS testing, field monitoring, positioning experiments, and exploring GNSS data**.

---

## 🚀 Features

* 📡 Real-time GPS/GNSS data monitoring
* 🛰️ Satellite count and position quality information
* 📍 Latitude and longitude
* 🧭 Course and compass direction
* 🚗 Current speed
* ⛰️ Altitude
* 📏 Distance from the trip starting point
* 🛣️ Route statistics
* 📊 Average moving speed
* ⚡ Minimum and maximum speed
* ⛰️ Minimum and maximum altitude
* 🕐 GPS/GNSS date and time
* 🌎 Configurable timezone
* 📅 Configurable date and time formats
* 📐 Configurable coordinate format
* 🚗 Configurable speed units
* 📏 Configurable altitude and distance units
* 💡 Configurable display brightness
* 🔋 Automatic display timeout to save battery
* ⚠️ GPS/GNSS communication warning
* 💾 Persistent configuration storage
* 🔧 Configurable GNSS baud rate
* 🔄 Trip reset function

---

# 🖥️ Screens

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens.gif" alt="Screens" width="70%">
</p>

### 📡 General

Provides an overview of the current GPS/GNSS status, including satellite information and position quality.

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

The firmware can calculate:

* Total route distance
* Average moving speed
* Minimum speed
* Maximum speed
* Minimum altitude
* Maximum altitude
* Moving time

Route distance is calculated by accumulating the distance between consecutive GNSS positions while the device is considered to be moving.

To reduce the effect of small GNSS position variations while stationary, the firmware only considers movement when the reported speed is above an internal **minimum moving speed**.

> **Note:** The minimum moving speed is an internal firmware parameter. It cannot currently be changed through the Settings menu.

### 🧭 Course

Displays the current direction of travel and compass direction calculated from the GNSS data.

### 🕐 Time

Displays the current date and time received from the GPS/GNSS module.

GNSS time is received in UTC and converted according to the timezone configured in the firmware.

The firmware also handles date changes when the converted time crosses midnight.

### 📈 Session Info

Provides information about the current GPS/GNSS session, including information related to the first valid fix and session operation.

### ℹ️ About

Information about the developer, selected baud rate, and TX and RX pins.

### ⚙️ Settings

Allows the user to configure the available firmware settings.

## Configuration Screens

The firmware includes dedicated screens for configuring the device and GPS/GNSS monitoring preferences:

| Screen             | Configuration                                   |
| -------------------| ------------------------------------------------|
| **Color**          | Theme color                                     |
| **Brightness**     | Display brightness                              |
| **Screen Timeout** | Turns off the screen if there is no interaction |
| **Coord. Format**  | Coordinate format                               |
| **Speed Unit**     | Speed measurement unit                          |
| **Altitude Unit**  | Altitude measurement unit                       |
| **Distance Unit**  | Distance measurement unit                       |
| **Timezone**       | Time zone used to display local date and time   |
| **Time Format**    | Time display formats                            |
| **Date Format**    | Date display formats                            |
| **Baud Rate**      | GPS/GNSS module communication baud rate         |
| **Reset Trip**     | Reset trip data and route statistics            |
| **Exit**           | Return to the General screen                    |


<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screens-config.gif" alt="GPS & GNSS Monitor Screens" width="70%">
</p>

---

# 🎛️ Button Controls

The StickS3 buttons are used to navigate between screens and configure the firmware.

### Button B

* **Short press:** Go to the next screen.
* **Long press:** Go to the previous screen.

### Button A

The function of Button A depends on the current screen.

It can be used to:

* Change display formats
* Change units
* Enter settings
* Change configuration values
* Confirm configuration changes
* Turn the display back on
* Dismiss the GPS/GNSS communication warning

Inside the Settings menu:

* **Button B:** Navigate between settings.
* **Button A:** Change or confirm the selected setting.

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

The firmware monitors the GPS/GNSS communication and displays a warning if valid information from the module is not received for a certain period of time.

This can be useful for identifying situations such as:

* 🔌 Disconnected GPS/GNSS modules
* 🔧 Incorrect UART wiring
* ⚙️ Incorrect baud rate
* 📡 Temporary communication problems
* 🔋 GPS/GNSS module power interruptions

The warning **does not stop the firmware**.

It can simply be ignored by pressing Button A, allowing the user to continue using the firmware.

> **Important:** The timeout used to trigger this warning is an **internal firmware parameter** and cannot currently be changed through the Settings menu.

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screen-warning.jpg" alt="GPS & GNSS Monitor Screens" width="50%">
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/screen-general-warning.jpg" alt="GPS & GNSS Monitor Screens" width="50%">
</p>

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

# 🔌 GPS/GNSS Connection

Connect the GPS/GNSS module to the StickS3 UART as follows:

| GPS/GNSS Module | StickS3                   |
| --------------- | --------------------------- |
| **TX**          | **GPIO 44 (RX)**            |
| **RX**          | **GPIO 43 (TX)**            |
| **GND**         | **GND**                     |
| **VCC**         | **Compatible power supply** |

<p align="center">
  <img src="https://raw.githubusercontent.com/viniciushnf/M5StickS3-GPS-GNSS-Monitor/refs/heads/main/images/wiring-diagram.png" alt="GPS & GNSS Monitor Screens" width="70%">
</p>

### UART Connection

The communication direction is crossed:

```text
GPS/GNSS TX  →  StickS3 GPIO 44 (RX)
GPS/GNSS RX  →  StickS3 GPIO 43 (TX)
GPS/GNSS GND →  StickS3 GND
GPS/GNSS VCC →  Compatible power supply
```

> ⚠️ Always verify the voltage requirements of your GPS/GNSS module before connecting it to the StickS3.

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

Configuration values are stored using the ESP32's non-volatile storage.

This means that settings normally remain available after:

* Restarting the device
* Powering the device off and on
* Installing a normal firmware update without completely erasing the flash

The firmware also initializes safe default values when necessary.

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

GPS & GNSS Monitor is also available through **M5Burner**.

The firmware name is:

```text
GPS & GNSS Monitor
```

### Installation using M5Burner

1. Download and install M5Burner.
2. Open M5Burner.
3. Search for:

```text
GPS & GNSS Monitor
```

4. Select the firmware.
5. Connect your StickS3 to the computer using USB.
6. Select the corresponding COM port.
7. Start the flashing process.
8. Wait until the installation is completed.
9. Restart the device.
10. Connect your GPS/GNSS module.

---

# 🧪 Testing

I performed several tests with the firmware and GPS/GNSS module to verify the main functions.

The tests included:

* 📡 GPS/GNSS communication
* 🛰️ Satellite information
* 📍 Position
* 🚗 Speed
* ⛰️ Altitude
* 📏 Distance
* 🛣️ Route statistics
* 🧭 Course
* 🕐 Date and time
* 🌎 Timezone conversion
* ⚙️ Configuration settings
* 💾 Persistent settings
* 🔋 Display timeout
* ⚠️ GPS/GNSS communication warning

The firmware worked correctly during these tests.

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