# Test Plan – Smart Classroom Comfort Monitor

## How to Run the Tests

```bash
gcc -o main src/main.c -lpigpio -lrt -lpthread -lm
sudo ./main test
```

The automated suite runs T1–T13 and prints `PASS` or `FAIL` for each one, followed by a summary (`Tests passed: X/13`). Tests T14–T20 are manual tests run through the program menu (`sudo ./main`).

Default acceptable range: **68°F–76°F, inclusive**.

## Automated Tests (`./main test`)

| ID  | Purpose | Input | Expected Output | Actual Output | Result |
|-----|---------|-------|-----------------|---------------|--------|
| T1  | Normal value inside range | 75°F | NORMAL (`isOutOfRange` = 0) | NORMAL | PASS |
| T2  | Below minimum | 64°F | WARNING (`isOutOfRange` = 1) | WARNING | PASS |
| T3  | Above maximum (decimal) | 78.5°F | WARNING | WARNING | PASS |
| T4  | Just below minimum | 67°F | WARNING | WARNING | PASS |
| T5  | Minimum boundary | 68°F | NORMAL | NORMAL | PASS |
| T6  | Inside range | 70°F | NORMAL | NORMAL | PASS |
| T7  | Maximum boundary | 76°F | NORMAL | NORMAL | PASS |
| T8  | Just above maximum | 77°F | WARNING | WARNING | PASS |
| T9  | Average calculation | 68, 69, 70, 71, 100 | 75.6°F | 75.6°F | PASS |
| T10 | Average with no readings | (empty) | 0, no crash | 0, no crash | PASS |
| T11 | New range 70–80°F | 79°F | NORMAL | NORMAL | PASS |
| T12 | New range 70–80°F | 68°F | WARNING | WARNING | PASS |
| T13 | LED hardware | LED test | LED on for 2 s, then off | LED lit for 2 s, then off | PASS |

**Summary: 13/13 tests passed.**

Evidence: *(insert photo/screenshot of `sudo ./main test` output on the Raspberry Pi here)*

## Manual Tests (program menu, `sudo ./main`)

| ID  | Purpose | Input | Expected Output | Actual Output | Result |
|-----|---------|-------|-----------------|---------------|--------|
| T14 | Example from assignment | 72 | NORMAL, LED OFF | NORMAL, LED OFF | PASS |
| T15 | Example from assignment | 79 | WARNING, LED ON | WARNING, LED ON | PASS |
| T16 | Example from assignment | 65 | WARNING, LED ON | WARNING, LED ON | PASS |
| T17 | Invalid text input | abc | "Invalid temperature" message, nothing saved | "Invalid temperature" message, nothing saved | PASS |
| T18 | Unrealistic value | 200 | "Invalid temperature" message, nothing saved | "Invalid temperature" message, nothing saved | PASS |
| T19 | Invalid range change | min 80, max 70 | "Minimum must be less than maximum", range unchanged | Range unchanged | PASS |
| T20 | File logging | Enter 72 then 79 | `data/temperature_log.csv` has both rows with statuses; `status.txt` says WARNING | Both rows saved; `status.txt` = WARNING | PASS |

## Regression Testing After Changes

The legacy lab program hard-coded the range as 68–76°F inside `isOutOfRange()`. We changed it to use adjustable `minTemp` / `maxTemp` variables so the client can change the range (FR8).

To confirm the change worked **and** nothing else broke:

- **T11 and T12** verify the new requirement: after changing the range to 70–80°F, 79°F becomes NORMAL and 68°F becomes WARNING.
- **T1–T8** were re-run after the change with the default range and still pass, showing the original behavior is unchanged, including both boundaries (T5, T7) and the values just outside them (T4, T8).
- **T10** was added after fixing a divide-by-zero bug in `calculateAverage()`; it confirms the program no longer crashes when there are no readings.

All tests were run on the Raspberry Pi 4B after the final code changes.
