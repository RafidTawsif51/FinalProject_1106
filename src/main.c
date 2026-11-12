#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef NO_PI
#include <pigpio.h>
#endif

#define MAX_READINGS    100
#define LED_PIN         17                      /* GPIO 17 = physical pin 11 */
#define LOG_FILE        "data/temperature_log.csv"
#define STATUS_FILE     "status.txt"
#define DEFAULT_MIN     68.0
#define DEFAULT_MAX     76.0
#define TEMP_LOW_LIMIT  -50.0                   /* reject impossible readings */
#define TEMP_HIGH_LIMIT 150.0

/* Acceptable range. Was hard-coded 68/76 in the lab-5 code; now changeable (FR8). */
double minTemp = DEFAULT_MIN;
double maxTemp = DEFAULT_MAX;

double readings[MAX_READINGS];
int readingCount = 0;

/* ================= Lab 5 functions ================= */

double calculateAverage(double readings[], int count) {        /* FR5 */
    if (count == 0) return 0;                  
    double total = 0;
    for (int i = 0; i < count; i++) total += readings[i];
    return total / count;
}

void displayReadings(double readings[], int count) {
    printf("Temperature Readings:\n");
    for (int i = 0; i < count; i++) printf("%.1f F\n", readings[i]);
}

int isOutOfRange(double temperature) {                          /* FR2 */
    if (temperature > maxTemp || temperature < minTemp)
        return 1;
    else
        return 0;
}

void checkReadings(double readings[], int count) {
    printf("\nReadings requiring attention:\n");
    for (int i = 0; i < count; i++) {
        if (isOutOfRange(readings[i])) printf("%.1f F\n", readings[i]);
    }
}

/* ================= Hardware (LED) ================= */

int ledInit(void) {
#ifdef NO_PI
    return 0;
#else
    if (gpioInitialise() < 0) return -1;
    gpioSetMode(LED_PIN, PI_OUTPUT);
    gpioWrite(LED_PIN, 0);
    return 0;
#endif
}

void setLED(int on) {                                           /* FR4 */
#ifdef NO_PI
    printf("[simulated LED %s]\n", on ? "ON" : "OFF");
#else
    gpioWrite(LED_PIN, on ? 1 : 0);
#endif
}

void ledShutdown(void) {
#ifndef NO_PI
    gpioWrite(LED_PIN, 0);
    gpioTerminate();
#endif
}

/* ================= Files ================= */

void logReading(double t, const char *status) {                 /* FR6 */
    FILE *f = fopen(LOG_FILE, "a");
    if (f == NULL) {
        printf("Could not open %s (run from the repo folder).\n", LOG_FILE);
        return;
    }
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)                          /* new file: add a header */
        fprintf(f, "temperature_f,status,min_f,max_f\n");
    fprintf(f, "%.1f,%s,%.1f,%.1f\n", t, status, minTemp, maxTemp);
    fclose(f);
}

void writeStatus(const char *status) {                          /* FR7 */
    FILE *f = fopen(STATUS_FILE, "w");
    if (f == NULL) {
        printf("Could not open %s.\n", STATUS_FILE);
        return;
    }
    fprintf(f, "%s\n", status);
    fclose(f);
}

/* ================= Input ================= */


int readNumber(const char *prompt, double *out) {
    char line[64];
    char *end;

    printf("%s", prompt);
    fflush(stdout);
    if (fgets(line, sizeof line, stdin) == NULL) return -1;

    line[strcspn(line, "\n")] = '\0';
    if (line[0] == '\0') return 0;

    *out = strtod(line, &end);
    if (end == line) return 0;                  /* no number at all */
    while (*end == ' ') end++;
    return *end == '\0';                        /* reject "72abc" */
}

/* ================= Menu actions ================= */

void enterTemperature(void) {                                   /* FR1, FR3 */
    double t;
    int r = readNumber("Enter temperature (F): ", &t);

    if (r != 1 || t < TEMP_LOW_LIMIT || t > TEMP_HIGH_LIMIT) {
        printf("Invalid temperature. Enter a number between %.0f and %.0f.\n",
               TEMP_LOW_LIMIT, TEMP_HIGH_LIMIT);
        return;
    }
    if (readingCount >= MAX_READINGS) {
        printf("Reading storage is full (%d).\n", MAX_READINGS);
        return;
    }

    readings[readingCount++] = t;

    int warning = isOutOfRange(t);
    const char *status = warning ? "WARNING" : "NORMAL";

    setLED(warning);
    logReading(t, status);
    writeStatus(status);

    printf("%.1f F -> %s (LED %s)\n", t, status, warning ? "ON" : "OFF");
}

void showSummary(void) {
    if (readingCount == 0) {
        printf("No readings recorded yet.\n");
        return;
    }
    displayReadings(readings, readingCount);
    printf("\nAverage temperature: %.1f F\n",
           calculateAverage(readings, readingCount));
    checkReadings(readings, readingCount);
}

void changeRange(void) {                                        /* FR8 */
    double lo, hi;

    if (readNumber("New minimum (F): ", &lo) != 1 ||
        readNumber("New maximum (F): ", &hi) != 1) {
        printf("Invalid input. Range not changed.\n");
        return;
    }
    if (lo >= hi) {
        printf("Minimum must be less than maximum. Range not changed.\n");
        return;
    }
    minTemp = lo;
    maxTemp = hi;
    printf("Acceptable range is now %.1f-%.1f F.\n", minTemp, maxTemp);
}

/* ================= Test suite (from lab 5 and extended) ================= */

int testsRun = 0;
int testsPassed = 0;

void recordTest(const char *testName, int passed) {
    testsRun++;
    if (passed) {
        testsPassed++;
        printf("PASS: %s\n", testName);
    } else {
        printf("FAIL: %s\n", testName);
    }
}

void resetRange(void) {
    minTemp = DEFAULT_MIN;
    maxTemp = DEFAULT_MAX;
}

/* T1-T8: range checks with the default 68-76 F range */
void test_T1(void) { recordTest("T1 - 75 F is NORMAL",            isOutOfRange(75.0) == 0); }
void test_T2(void) { recordTest("T2 - 64 F is WARNING",           isOutOfRange(64.0) == 1); }
void test_T3(void) { recordTest("T3 - 78.5 F is WARNING",         isOutOfRange(78.5) == 1); }
void test_T4(void) { recordTest("T4 - 67 F (just below min) is WARNING", isOutOfRange(67.0) == 1); }
void test_T5(void) { recordTest("T5 - 68 F (min) is NORMAL",      isOutOfRange(68.0) == 0); }
void test_T6(void) { recordTest("T6 - 70 F is NORMAL",            isOutOfRange(70.0) == 0); }
void test_T7(void) { recordTest("T7 - 76 F (max) is NORMAL",      isOutOfRange(76.0) == 0); }
void test_T8(void) { recordTest("T8 - 77 F (just above max) is WARNING", isOutOfRange(77.0) == 1); }

/* T9: average of the legacy example readings */
void test_T9(void) {
    double r[5] = {68.0, 69.0, 70.0, 71.0, 100.0};
    recordTest("T9 - average of 68,69,70,71,100 is 75.6",
               fabs(calculateAverage(r, 5) - 75.6) < 0.001);
}

/* T10: average with no readings does not crash */
void test_T10(void) {
    double r[1];
    recordTest("T10 - average of 0 readings returns 0", calculateAverage(r, 0) == 0);
}

/* T11-T12: new client requirement, range changed to 70-80 F */
void test_T11(void) {
    minTemp = 70.0; maxTemp = 80.0;
    recordTest("T11 - range 70-80: 79 F is NORMAL", isOutOfRange(79.0) == 0);
    resetRange();
}
void test_T12(void) {
    minTemp = 70.0; maxTemp = 80.0;
    recordTest("T12 - range 70-80: 68 F is WARNING", isOutOfRange(68.0) == 1);
    resetRange();
}

/* T13: LED test - lights the LED for 2 s so you can see it works */
void test_T13_led(void) {
    printf("LED should turn ON for 2 seconds...\n");
    setLED(1);
#ifndef NO_PI
    gpioDelay(2000000);
#endif
    setLED(0);
    recordTest("T13 - LED toggled (confirm visually)", 1);
}

void runAllTests(void) {
    resetRange();
    printf("========================================\n");
    printf("CSE 1106 TEST SUITE\n");
    printf("========================================\n\n");

    test_T1(); test_T2(); test_T3(); test_T4();
    test_T5(); test_T6(); test_T7(); test_T8();
    test_T9(); test_T10(); test_T11(); test_T12();
    test_T13_led();

    printf("\nTests passed: %d/%d\n", testsPassed, testsRun);
}

/* ================= Main ================= */

int main(int argc, char *argv[]) {
    if (ledInit() < 0) {
        printf("pigpio failed to start. Run with sudo, and if it still fails "
               "run: sudo killall pigpiod\n");
        return 1;
    }

    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        runAllTests();
        ledShutdown();
        return testsPassed == testsRun ? 0 : 1;
    }

    while (1) {
        double choice;
        int r;

        printf("\n========================================\n");
        printf("SMART CLASSROOM COMFORT MONITOR\n");
        printf("========================================\n");
        printf("Acceptable range: %.1f-%.1f F\n", minTemp, maxTemp);
        printf("1) Enter temperature\n");
        printf("2) Show readings, average, and warnings\n");
        printf("3) Change acceptable range\n");
        printf("4) Exit\n");

        r = readNumber("Choice: ", &choice);
        if (r == -1) break;
        if (r == 0 || choice != (int)choice) {
            printf("Please enter 1, 2, 3, or 4.\n");
            continue;
        }

        switch ((int)choice) {
            case 1: enterTemperature(); break;
            case 2: showSummary();      break;
            case 3: changeRange();      break;
            case 4: goto done;
            default: printf("Please enter 1, 2, 3, or 4.\n");
        }
    }

done:
    ledShutdown();
    printf("Goodbye.\n");
    return 0;
}
