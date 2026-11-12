# Change Log – Smart Classroom Comfort Monitor

## 2026-11-12 – Version 1.0: Final project program

Started from the legacy `temp_monitor.c` program from lab and extended it to meet the project requirements.

### Fixed
- **`isOutOfRange()` used hard-coded 68 and 76.** Replaced with `minTemp` and `maxTemp` variables so the range can change (FR8).
- **`calculateAverage()` could divide by zero** when there were no readings. It now returns 0 in that case.

### Added
- Menu that lets the user enter readings, view readings and the average, change the range, and exit (FR1, FR5, FR8).
- NORMAL / WARNING status message for every reading (FR3).
- LED warning indicator on GPIO 17 using the pigpio library (FR4).
- Logging of every reading and its status to `data/temperature_log.csv` (FR6).
- Latest status saved to `status.txt` (FR7).
- Input validation: rejects text, blank input, and values outside -50°F to 150°F.
- Range validation: the minimum must be less than the maximum.
- Tests T9–T13 (average, empty average, new range 70–80°F, LED) alongside the original T1–T8.
- `./main test` command to run the full test suite.
- `-DNO_PI` build option to test on a laptop without the Pi.

### Kept from the legacy program
- `displayReadings()`, `checkReadings()`, `recordTest()`, and tests T1–T8.

### Testing
- All 13 automated tests pass on the Raspberry Pi 4B (see `tests/test_plan.md`).
- T1–T8 were re-run after the range change to confirm the original behavior did not change.

## 2026-11-12 – Repository cleanup
- Added `.gitignore` so the compiled `main` program is not committed.

## 2026-11-05 – Project setup
- Created the GitHub repository with the recommended folder structure.
- Wrote the README and `docs/requirements.md`.
