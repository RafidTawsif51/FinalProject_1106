# Smart Classroom Comfort Monitor

CSE 1106 Final Project – Option 1

A Raspberry Pi 4B application written in C that takes classroom temperature readings in Fahrenheit, checks them against an acceptable range (default 68°F–76°F, inclusive), shows a NORMAL or WARNING status, lights an LED for warnings, logs every reading to a file, and reports the average temperature. The acceptable range can be changed while the program is running.

## Team
- Rafid Tawsif
- Ruby Osorio

## Hardware
- Raspberry Pi 4B
- LED + 1kΩ resistor on GPIO 17 (physical pin 11)
- LED short leg to GND (physical pin 6)

## Build and Run (Raspberry Pi)

Run these from the repository folder:

```bash
gcc -o main src/main.c -lpigpio -lrt -lpthread -lm
sudo ./main          # run the program
sudo ./main test     # run the automated test suite
```

If pigpio fails to start, run `sudo killall pigpiod` and try again.

To test on a laptop without a Pi (the LED is simulated):

```bash
gcc -DNO_PI -o main src/main.c -lm
./main test
```

## Using the Program

```
1) Enter temperature
2) Show readings, average, and warnings
3) Change acceptable range
4) Exit
```

| Temperature | Status  | LED |
|-------------|---------|-----|
| 72°F        | NORMAL  | OFF |
| 79°F        | WARNING | ON  |
| 65°F        | WARNING | ON  |

## Repository Layout
- `src/main.c` – C source code
- `docs/requirements.md` – functional and non-functional requirements
- `docs/design.md` – architecture and hardware/software interaction
- `docs/change_log.md` – history of changes
- `tests/test_plan.md` – test cases and results
- `data/temperature_log.csv` – saved readings and statuses
- `status.txt` – latest status (NORMAL or WARNING)
