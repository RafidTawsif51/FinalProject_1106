# Smart Classroom Comfort Monitor

CSE 1106 Final Project – Option 1

A Raspberry Pi 4B application written in C that takes classroom temperature
readings in Fahrenheit, checks them against an acceptable range (default
68°F–76°F inclusive), shows a NORMAL or WARNING status, lights an LED on
warnings, logs every reading, and reports the average temperature.

## Team
- Rafid Tawsif
- Ruby Osorio

## Hardware
- Raspberry Pi 4B
- LED + 1kΩ resistor on GPIO 17 (physical pin 11), GND on pin 6

## Build & Run
gcc -o main src/main.c -lpigpio
sudo ./main

## Repo Layout
- src/ – C source code
- docs/ – requirements, design, change log
- tests/ – test plan
- data/temperature_log.csv – saved readings
- status.txt – latest status (NORMAL or WARNING)
