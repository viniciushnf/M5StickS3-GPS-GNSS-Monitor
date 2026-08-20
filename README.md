# GPS & GNSS Monitor for M5StickS3

GPS & GNSS Monitor is a firmware for the M5StickS3 designed to display and monitor real-time data received from a connected GPS/GNSS module.

The firmware provides GNSS information, trip and route statistics, configurable display and measurement settings, and persistent configuration storage using the ESP32 NVS.

## About the Firmware

GPS & GNSS Monitor receives NMEA data from a GPS/GNSS module through the M5StickS3 hardware UART and processes it using the TinyGPSPlus library.

The firmware can display the following information:

- Number of satellites
- HDOP and position quality
- Latitude and longitude
- Altitude
- Current speed
- Course and compass direction
- Date and time
- Distance from the trip starting point
- Route distance
- Average moving speed
- Minimum and maximum speed
- Minimum and maximum altitude
- First-fix and session information

### GPS/GNSS Connection

The firmware uses the following M5StickS3 GPIOs for the GNSS module:

| GNSS Module | M5StickS3 |
|---|---|
| TX | GPIO 44 (RX) |
| RX | GPIO 43 (TX) |

The UART communication uses 8 data bits, no parity, and 1 stop bit (`SERIAL_8N1`).

### Route and Trip Statistics

The firmware maintains two different distance measurements:

- **Distance**: straight-line distance between the trip starting point and the current position.
- **Route Stats**: accumulated distance between consecutive GNSS positions while the device is moving.

Route distance and moving time are only recorded when the GNSS-reported speed is above the configured minimum moving speed. This helps prevent small GPS position variations while stationary from being counted as movement.

The **Average Moving Speed** is calculated from the accumulated route distance and the total time spent moving.

### Communication Timeout

The firmware monitors valid GNSS time updates to detect communication problems.

By default, a warning is shown when no valid GNSS time update is received for **15 seconds**. This timeout can be configured in the firmware settings.

### Configuration Settings

The firmware includes configurable settings for:

- GNSS baud rate
- Theme color
- Coordinate format
- Speed unit
- Altitude unit
- Distance unit
- Timezone
- Time format
- Date format
- Display brightness
- GNSS communication timeout
- Minimum moving speed

### Persistent Settings

Settings are stored using the ESP32 **NVS (Non-Volatile Storage)** through the `Preferences` library.

This means the configured values remain stored after:

- Rebooting the M5StickS3
- Turning the device off and on again
- Uploading a new firmware normally without erasing the entire flash

On the first startup, or when no valid persistent settings are found, the firmware loads the default settings and asks the user to select the GNSS module baud rate.

Invalid stored settings are automatically replaced with safe default values.

### Button Controls

The M5StickS3 buttons are used to navigate through the firmware interface.

#### Button B

- **Short press:** move to the next screen.
- **Long press:** move to the previous screen.

#### Button A

Button A behavior depends on the current screen. It is used to:

- Change the format or unit of the current screen
- Enter the Settings menu
- Change a setting
- Confirm certain actions
- Dismiss the communication warning

In the Settings menu, Button B navigates between settings and Button A changes or confirms the selected option.

### Time and Timezone

GNSS time is received in UTC and converted according to the configured timezone. The firmware also adjusts the calendar date when the timezone conversion crosses midnight.

### Supported GNSS Modules

The firmware is designed to work with GNSS modules that provide standard NMEA data over UART.

It was developed and tested with the **REYAX RYS352A**, but other compatible NMEA GPS/GNSS modules may also work when configured with a supported baud rate and connected to the correct UART pins.

## Installation Using Arduino IDE

To compile and upload the firmware from source, you must first install the **M5StickS3 board support package** and the required libraries in Arduino IDE.

The official M5Stack documentation provides the required instructions for configuring the M5StickS3 in Arduino IDE:

[How to program M5StickS3 with Arduino IDE](https://docs.m5stack.com/en/arduino/m5sticks3/program)

After completing the board and library installation:

1. Open `GPS_GNSS_Monitor.ino` in Arduino IDE.
2. Select the M5StickS3 board.
3. Connect the M5StickS3 to the computer through USB.
4. Compile and upload the firmware.
5. On the first startup, select the baud rate used by your GPS/GNSS module when prompted by the firmware.

## Installation Using the `.bin` Firmware File

A precompiled `.merged.bin` firmware image is provided in this repository for users who do not want to compile the source code.

For the easiest flashing process, it is recommended to use **Google Chrome** with the official Espressif web-based esptool:

[Espressif esptool-js](https://espressif.github.io/esptool-js/)

### Step 1 — Download the Firmware

Download the `GPS_GNSS_Monitor.merged.bin` file from this repository.

### Step 2 — Enter Programming Mode

Put the M5StickS3 into programming/download mode:

1. Disconnect the M5StickS3 from the computer if necessary.
2. Press and hold the **Power button**.
3. Keep holding it until the indicator light starts flashing.
4. The M5StickS3 is now in programming mode.

### Step 3 — Connect the M5StickS3

Connect the M5StickS3 to the computer using USB.

Open the [Espressif esptool-js](https://espressif.github.io/esptool-js/) page in Google Chrome and connect to the M5StickS3 using a serial baud rate of **115200**.

### Step 4 — Select the Firmware File

Select:

```text
GPS_GNSS_Monitor.merged.bin
```

Use the following flashing parameters:

| Parameter | Value |
|---|---|
| Flash Address | `0x0` |
| Flash Mode | `DIO` |
| Flash Frequency | `80MHz` |
| Flash Size | `8MB` |

Then click **Program**.

### Step 5 — Wait for the Upload to Finish

Wait until esptool-js reports that the flashing process has completed successfully.

Do not disconnect the M5StickS3 while the firmware is being written.

After the upload is complete, restart the M5StickS3.

### First Startup After Installation

Regardless of whether the firmware was installed through Arduino IDE or using the `.bin` file, the **first startup requires the user to select the baud rate of the GPS/GNSS module being used**.

This is necessary because different GNSS modules may use different UART baud rates.

For example, if your module communicates at 115200 baud, select **115200** during the first startup configuration.

## Source Code

The main firmware source code is available in:

```text
GPS_GNSS_Monitor.ino
```

The precompiled firmware image is available in:

```text
GPS_GNSS_Monitor.merged.bin
```

## License

This project is released under the **MIT License**.
