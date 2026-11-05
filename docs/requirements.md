# Requirements – Smart Classroom Comfort Monitor

## Functional Requirements
| ID  | Requirement |
|-----|-------------|
| FR1 | The user can enter temperature readings in °F. |
| FR2 | The system checks if each reading is within the acceptable range (default 68–76°F, inclusive). |
| FR3 | The system displays NORMAL if in range, WARNING if out of range. |
| FR4 | The LED on GPIO 17 turns ON for WARNING and OFF for NORMAL. |
| FR5 | The system calculates and displays the average of all recorded readings. |
| FR6 | Each reading and its status is saved to data/temperature_log.csv. |
| FR7 | The latest status is written to status.txt. |
| FR8 | The user can change the minimum and maximum acceptable temperature. |

## Non-Functional Requirements
- NFR1: Written in C and runs on Raspberry Pi 4B using the pigpio library.
- NFR2: Invalid input (letters, blank) is rejected without crashing.
- NFR3: The menu is simple enough to use without instructions.
- NFR4: The range limits (68 and 76) are not hard-coded in multiple places.

## Inputs / Outputs
- Input: temperature (°F), menu choice, new min/max range
- Output: status message, LED state, average, CSV log, status.txt

## Example
| Temperature | Status  | LED |
|-------------|---------|-----|
| 72°F        | NORMAL  | OFF |
| 79°F        | WARNING | ON  |
| 65°F        | WARNING | ON  |
| 68°F        | NORMAL  | OFF |  (boundary)
| 76°F        | NORMAL  | OFF |  (boundary)
