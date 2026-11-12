# System Design – Smart Classroom Comfort Monitor

## Overview

The system is a C program running on a Raspberry Pi 4B. The user types classroom temperature readings into a terminal menu. The program decides whether each reading is NORMAL or WARNING, turns an LED on for warnings, saves every reading to a file, and reports the average.

## Architecture

```
            +------------------+
  User  --> |  Terminal menu   |   (keyboard input / screen output)
            +--------+---------+
                     |
                     v
            +------------------+        +------------------------------+
            |   C program      | -----> | data/temperature_log.csv     |
            |   src/main.c     |        | (every reading + status)     |
            |                  | -----> | status.txt                   |
            |  - input check   |        | (latest: NORMAL or WARNING)  |
            |  - range check   |        +------------------------------+
            |  - average       |
            +--------+---------+
                     | pigpio library
                     v
            +------------------+
            |  GPIO 17 (pin 11)|
            +--------+---------+
                     v
             1kΩ resistor -> LED -> GND (pin 6)
```

## How Software and Hardware Interact

1. The user enters a temperature in °F.
2. `isOutOfRange()` compares it to `minTemp` and `maxTemp` (default 68–76°F, inclusive).
3. If out of range, the status is **WARNING** and `setLED(1)` sets GPIO 17 HIGH (3.3V). Current flows through the 1kΩ resistor and the LED lights up.
4. If in range, the status is **NORMAL** and `setLED(0)` sets GPIO 17 LOW (0V), turning the LED off.
5. The LED always shows the status of the most recent reading.
6. The reading and status are appended to `data/temperature_log.csv`, and `status.txt` is overwritten with the latest status.

The 1kΩ resistor limits current through the LED to a few milliamps, which protects both the LED and the Pi's GPIO pin.

## Hardware

| Component | Connection |
|-----------|------------|
| Raspberry Pi 4B | Runs the program |
| LED (long leg / anode) | Through 1kΩ resistor to GPIO 17 (physical pin 11) |
| LED (short leg / cathode) | GND (physical pin 6) |
| 1kΩ resistor | Brown-Black-Red (4-band) or Brown-Black-Black-Brown (5-band) |

## Software Modules

| Function | Purpose | Requirement |
|----------|---------|-------------|
| `main()` | Shows the menu and routes choices; `./main test` runs the test suite | – |
| `readNumber()` | Reads and validates numeric keyboard input | NFR2 |
| `enterTemperature()` | Takes a reading, sets status, LED, and saves it | FR1, FR3 |
| `isOutOfRange()` | Checks a temperature against `minTemp` / `maxTemp` | FR2 |
| `setLED()` | Turns the LED on/off through pigpio | FR4 |
| `calculateAverage()` | Averages all stored readings | FR5 |
| `displayReadings()`, `checkReadings()`, `showSummary()` | Show all readings, the average, and readings needing attention | FR3, FR5 |
| `logReading()` | Appends reading + status to the CSV log | FR6 |
| `writeStatus()` | Writes the latest status to `status.txt` | FR7 |
| `changeRange()` | Lets the user change the acceptable range | FR8 |
| `runAllTests()`, `recordTest()` | Automated test suite (T1–T13) | Testing |

## Data Files

**`data/temperature_log.csv`**
```
temperature_f,status,min_f,max_f
72.0,NORMAL,68.0,76.0
79.0,WARNING,68.0,76.0
```
The range is saved with each reading, so the log still makes sense after the range is changed.

**`status.txt`** contains a single word: `NORMAL` or `WARNING`.

## Design Decisions

- **Adjustable range instead of hard-coded numbers.** The legacy code used `68` and `76` directly in `isOutOfRange()`. They are now variables so a new client requirement only needs a menu option, not a code change.
- **Input validation.** Text, blank input, and impossible values (below -50°F or above 150°F) are rejected so bad data never reaches the log or the average.
- **Built-in test mode.** `./main test` runs every test with one command, which makes it easy to re-test after any change.
- **Laptop build option.** Compiling with `-DNO_PI` replaces the LED with printed messages so the logic can be tested without a Raspberry Pi.
