# GPS & GNSS Monitor for M5StickS3

GPS & GNSS Monitor transforms your **M5StickS3** into a compact, real-time GPS/GNSS monitoring device.

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

The firmware provides several dedicated screens for monitoring different aspects of the GPS/GNSS data.

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

### ℹ️ Session Info

Provides information about the current GPS/GNSS session, including information related to the first valid fix and session operation.

### ⚙️ Settings

Allows the user to configure the available firmware settings.

---

# 🎛️ Button Controls

The M5StickS3 buttons are used to navigate between screens and configure the firmware.

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

Different GPS/GNSS modules can provide different levels of:

* 📍 Position accuracy
* 🛰️ Satellite acquisition performance
* ⚡ Fix acquisition speed
* 📡 Signal sensitivity
* 🔄 Update rate

Therefore, the overall performance of the system depends not only on the firmware, but also on the GPS/GNSS module, antenna, satellite visibility, environment, and signal conditions.

---

# 🔌 GPS/GNSS Connection

Connect the GPS/GNSS module to the M5StickS3 UART as follows:

| GPS/GNSS Module | M5StickS3                   |
| --------------- | --------------------------- |
| **TX**          | **GPIO 44 (RX)**            |
| **RX**          | **GPIO 43 (TX)**            |
| **GND**         | **GND**                     |
| **VCC**         | **Compatible power supply** |

### UART Connection

The communication direction is crossed:

```text
GPS/GNSS TX  →  M5StickS3 GPIO 44 (RX)
GPS/GNSS RX  →  M5StickS3 GPIO 43 (TX)
GPS/GNSS GND →  M5StickS3 GND
GPS/GNSS VCC →  Compatible power supply
```

> ⚠️ Always verify the voltage requirements of your GPS/GNSS module before connecting it to the M5StickS3.

---

# 🧰 Hardware Assembly

For my hardware setup, I soldered the necessary pins to a small PCB and wired the GPS/GNSS module and the M5StickS3 together.

This creates a compact assembly where the GPS/GNSS module and the M5StickS3 remain firmly attached to each other.

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

According to the manufacturer, the RYS352A supports multiple GNSS systems and provides NMEA output over UART, with navigation updates of up to 10 Hz.

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

# ⚙️ Configuration

The firmware provides several configurable options.

### 🎨 Display

* Theme color
* Display brightness
* Display timeout

### 📍 Position

* Coordinate format

### 🚗 Speed

* Speed unit

### ⛰️ Altitude

* Altitude unit

### 📏 Distance

* Distance unit

### 🕐 Time

* Timezone
* Time format
* Date format

### 📡 GPS/GNSS

* GNSS baud rate

### 🔄 Trip

* Reset trip statistics

---

# 💾 Persistent Settings

Configuration values are stored using the ESP32's non-volatile storage.

This means that settings normally remain available after:

* Restarting the device
* Powering the device off and on
* Installing a normal firmware update without completely erasing the flash

The firmware also initializes safe default values when necessary.

---

# 🟢 First Startup

After installing the firmware and connecting a GPS/GNSS module:

1. 🔌 Connect the GPS/GNSS module to the M5StickS3.
2. ⚙️ Make sure the selected GNSS baud rate matches the module.
3. 🛰️ Move to an area with good sky visibility.
4. ⏳ Wait for the GPS/GNSS receiver to acquire a FIX.
5. 📍 The position and other GNSS information will begin to appear on the display.

The first FIX can take longer depending on the GNSS module, satellite visibility, antenna, and current receiver conditions.

---

# 💻 Installation

There are three ways to install GPS & GNSS Monitor.

## 1. 📦 Install the `.bin` Firmware

The repository contains the precompiled firmware:

```text
GPS_GNSS_Monitor.bin
```

You can use a browser-based ESP flashing tool such as **esptool-js** to program the device.

[Open ESP Tool JS](https://espressif.github.io/esptool-js/)

### Steps

1. Download `GPS_GNSS_Monitor.bin` from this repository.
2. Put the M5StickS3 into programming mode.
3. Connect it to your computer through USB.
4. Open the ESP flashing tool: ([https://espressif.github.io/esptool-js/](https://espressif.github.io/esptool-js/)).
5. Select the downloaded `.bin` file.
6. Select the appropriate serial port.
7. Start the flashing process.
8. Restart the device.
9. Connect the GPS/GNSS module.
10. Select the correct GNSS baud rate on the first startup.

---

## 2. 🛠️ Install Using Arduino IDE

The complete source code is also provided in the repository.

Main source file:

```text
GPS_GNSS_Monitor.ino
```

### Requirements

* Arduino IDE
* M5StickS3 board support
* M5Unified library
* M5GFX library
* TinyGPSPlus library

### Installation

1. Install Arduino IDE.
2. Open the `GPS_GNSS_Monitor.ino` file.
3. Install the required libraries.
4. Select the **M5StickS3** board.
5. Connect the M5StickS3 using USB.
6. Select the correct serial port.
7. Compile and upload the firmware.
8. Restart the device.
9. Connect the GPS/GNSS module.
10. Configure the GNSS baud rate if necessary.

Official M5Stack Arduino documentation:

https://docs.m5stack.com/en/arduino/m5sticks3/program

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
5. Connect your M5StickS3 to the computer using USB.
6. Select the corresponding COM port.
7. Start the flashing process.
8. Wait until the installation is completed.
9. Restart the device.
10. Connect your GPS/GNSS module.

Official M5Burner documentation:

https://github.com/m5stack/m5-docs/blob/master/docs/en/related_documents/M5Burner.md

---

# 📚 Official M5Stack Documentation

The official M5Stack documentation for the StickS3 is available here:

https://docs.m5stack.com/en/core/StickS3

The Arduino programming guide is available here:

https://docs.m5stack.com/en/arduino/m5sticks3/program

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

# 🐉 Bruce Firmware with GPS Info

I also made available a **modified version of the Bruce firmware** that adds a dedicated:

```text
GPS Info
```

screen.

This screen provides several pieces of information received from the connected GPS/GNSS module and can be useful for users who want to inspect GNSS information while using Bruce.

If you are interested in installing this modified Bruce firmware, see the dedicated repository:

https://github.com/viniciushnf/M5StickS3-with-GPS-and-GNSS

---

# 📁 Repository Contents

The repository contains the main files required to use and develop the project.

```text
GPS_GNSS_Monitor.ino
GPS_GNSS_Monitor.bin
README.md
```

The repository also contains project images and other supporting files.

---

# 🙏 Acknowledgments

Special thanks to:

* **REYAX** for providing the RYS352A GPS/GNSS module used during development and testing.

---

## ⭐ If you find this project useful

If this project helps you with GPS/GNSS experimentation, navigation, development, or testing, consider giving the repository a ⭐ on GitHub.

---

# 📄 License

This project is released under the **MIT License**.