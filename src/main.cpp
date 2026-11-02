#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// =====================================================
// CHARGEWISE
// ESP32 Smart Charging Advisory Prototype - Phase 1
//
// Features:
// - SSD1306 OLED interface
// - DS18B20 temperature monitoring
// - Three selectable operating modes
// - Single push-button interface
// - Short press: Change operating mode
// - Long press: Change OLED display page
// - Three LED status indicators
//
// IMPORTANT:
// This prototype does NOT control actual charging.
// =====================================================


// ---------------- OLED CONFIGURATION ----------------

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);


// ---------------- GPIO CONFIGURATION ----------------

// OLED
constexpr uint8_t OLED_SDA = 26;
constexpr uint8_t OLED_SCL = 27;

// Temperature sensor
constexpr uint8_t TEMP_PIN = 4;

// Single push button
constexpr uint8_t BUTTON_PIN = 18;

// LEDs
constexpr uint8_t LED_GREEN = 25;
constexpr uint8_t LED_YELLOW = 32;
constexpr uint8_t LED_RED = 33;


// ---------------- TEMPERATURE SENSOR ----------------

OneWire oneWire(TEMP_PIN);
DallasTemperature temperatureSensor(&oneWire);


// ---------------- OPERATING MODES -------------------

enum ChargingMode {
    SAFE_MODE = 0,
    BALANCED_MODE,
    EMERGENCY_MODE,
    MODE_COUNT
};

ChargingMode selectedMode = SAFE_MODE;


// ---------------- SYSTEM STATES ---------------------

enum SystemState {
    SYSTEM_READY,
    TEMPERATURE_WARNING,
    SENSOR_ERROR
};

SystemState systemState = SENSOR_ERROR;


// Demonstration threshold only.
// Not a validated battery safety limit.

constexpr float DEMO_WARNING_TEMP_C = 40.0f;


// ---------------- SENSOR DATA ------------------------

float temperatureC = NAN;

bool temperatureValid = false;
bool conversionPending = false;

unsigned long conversionStartTime = 0;
unsigned long lastTemperatureRequest = 0;

constexpr unsigned long TEMP_INTERVAL_MS = 1000;
constexpr unsigned long CONVERSION_TIME_MS = 200;


// ---------------- BUTTON CONFIGURATION ---------------

constexpr unsigned long DEBOUNCE_MS = 35;
constexpr unsigned long LONG_PRESS_MS = 2000;

bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;

unsigned long lastButtonChange = 0;
unsigned long buttonPressStart = 0;


// ---------------- DISPLAY CONFIGURATION --------------

uint8_t displayPage = 0;

unsigned long lastDisplayUpdate = 0;

constexpr unsigned long DISPLAY_INTERVAL_MS = 250;


// ---------------- HELPER FUNCTIONS -------------------

const char* getModeName() {

    switch (selectedMode) {

        case SAFE_MODE:
            return "SAFE";

        case BALANCED_MODE:
            return "BALANCED";

        case EMERGENCY_MODE:
            return "EMERGENCY";

        default:
            return "UNKNOWN";
    }
}


const char* getStatusName() {

    switch (systemState) {

        case SYSTEM_READY:
            return "READY";

        case TEMPERATURE_WARNING:
            return "TEMP WARNING";

        case SENSOR_ERROR:
            return "SENSOR ERROR";

        default:
            return "UNKNOWN";
    }
}


// ---------------- SYSTEM STATE -----------------------

void updateSystemState() {

    if (!temperatureValid) {

        systemState = SENSOR_ERROR;

    } else if (temperatureC >= DEMO_WARNING_TEMP_C) {

        systemState = TEMPERATURE_WARNING;

    } else {

        systemState = SYSTEM_READY;
    }
}


// ---------------- LED CONTROL ------------------------

void updateLEDs() {

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    switch (systemState) {

        case SYSTEM_READY:

            digitalWrite(LED_GREEN, HIGH);
            break;

        case TEMPERATURE_WARNING:

            digitalWrite(LED_RED, HIGH);
            break;

        case SENSOR_ERROR:

            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_YELLOW, HIGH);
            break;
    }
}


// ---------------- TEMPERATURE MONITORING ------------

void readTemperature() {

    unsigned long now = millis();

    // Check whether an earlier conversion has completed.

    if (conversionPending) {

        if (now - conversionStartTime >= CONVERSION_TIME_MS) {

            float reading =
                temperatureSensor.getTempCByIndex(0);

            if (
                reading == DEVICE_DISCONNECTED_C ||
                reading < -55.0f ||
                reading > 125.0f ||
                !isfinite(reading)
            ) {

                temperatureValid = false;

            } else {

                temperatureC = reading;
                temperatureValid = true;
            }

            conversionPending = false;

            updateSystemState();
            updateLEDs();
        }

        return;
    }


    // Request a new conversion when the interval expires.

    if (now - lastTemperatureRequest >= TEMP_INTERVAL_MS) {

        temperatureSensor.requestTemperatures();

        conversionStartTime = now;
        lastTemperatureRequest = now;

        conversionPending = true;
    }
}


// ---------------- BUTTON HANDLING -------------------

void handleButton() {

    bool reading = digitalRead(BUTTON_PIN);

    unsigned long now = millis();


    // Detect changes in the physical button signal.

    if (reading != lastButtonReading) {

        lastButtonChange = now;
        lastButtonReading = reading;
    }


    // Apply debounce delay.

    if (now - lastButtonChange >= DEBOUNCE_MS) {

        if (reading != stableButtonState) {

            stableButtonState = reading;


            // Button pressed.

            if (stableButtonState == LOW) {

                buttonPressStart = now;
            }


            // Button released.

            else {

                unsigned long pressDuration =
                    now - buttonPressStart;


                // Long press: Change OLED page.

                if (pressDuration >= LONG_PRESS_MS) {

                    displayPage = (displayPage + 1) % 2;

                    Serial.println("Display page changed.");
                }


                // Short press: Change operating mode.

                else {

                    selectedMode = static_cast<ChargingMode>(
                        (selectedMode + 1) % MODE_COUNT
                    );

                    Serial.print("Selected mode: ");
                    Serial.println(getModeName());
                }
            }
        }
    }
}


// ---------------- OLED DISPLAY -----------------------

void drawHeader() {

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("CHARGEWISE");

    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
}


void drawMainPage() {

    drawHeader();

    display.setCursor(0, 15);

    display.print("Mode: ");
    display.println(getModeName());

    display.setCursor(0, 27);

    display.print("Temp: ");

    if (temperatureValid) {

        display.print(temperatureC, 1);
        display.println(" C");

    } else {

        display.println("SENSOR ERROR");
    }

    display.setCursor(0, 39);

    display.print("Status: ");
    display.println(getStatusName());

    display.setCursor(0, 53);

    display.println("Short: MODE Long: PAGE");
}


void drawInfoPage() {

    drawHeader();

    display.setCursor(0, 15);
    display.println("SYSTEM INFORMATION");

    display.setCursor(0, 27);
    display.println("Controller: ESP32");

    display.setCursor(0, 39);

    display.print("Sensor: ");

    display.println(
        temperatureValid ? "ONLINE" : "ERROR"
    );

    display.setCursor(0, 51);
    display.println("Charging: NOT CONNECTED");
}


void updateDisplay() {

    unsigned long now = millis();

    if (now - lastDisplayUpdate < DISPLAY_INTERVAL_MS) {
        return;
    }

    lastDisplayUpdate = now;

    display.clearDisplay();

    if (displayPage == 0) {

        drawMainPage();

    } else {

        drawInfoPage();
    }

    display.display();
}


// ---------------- SERIAL MONITOR --------------------

void printSystemStatus() {

    Serial.println();
    Serial.println("==============================");
    Serial.println("      CHARGEWISE STATUS");
    Serial.println("==============================");

    Serial.print("Mode: ");
    Serial.println(getModeName());

    Serial.print("Temperature: ");

    if (temperatureValid) {

        Serial.print(temperatureC, 2);
        Serial.println(" C");

    } else {

        Serial.println("INVALID");
    }

    Serial.print("System state: ");
    Serial.println(getStatusName());

    Serial.println("Charging hardware: NOT CONNECTED");

    Serial.println("==============================");
}


// ---------------- SETUP ------------------------------

void setup() {

    Serial.begin(115200);

    delay(300);

    Serial.println();
    Serial.println("==============================");
    Serial.println("      CHARGEWISE ONLINE");
    Serial.println("==============================");


    // Button initialization.

    pinMode(BUTTON_PIN, INPUT_PULLUP);


    // LED initialization.

    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    updateLEDs();


    // OLED initialization.

    Wire.begin(OLED_SDA, OLED_SCL);

    if (!display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDRESS
        )) {

        Serial.println("OLED initialization failed.");

        digitalWrite(LED_RED, HIGH);

        while (true) {
            delay(1000);
        }
    }


    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 10);
    display.println("CHARGEWISE");

    display.setCursor(0, 25);
    display.println("Initializing...");

    display.setCursor(0, 40);
    display.println("Please wait");

    display.display();


    // DS18B20 initialization.

    temperatureSensor.begin();

    temperatureSensor.setResolution(10);

    temperatureSensor.setWaitForConversion(false);


    // Start initial temperature conversion.

    temperatureSensor.requestTemperatures();

    conversionStartTime = millis();
    lastTemperatureRequest = millis();

    conversionPending = true;


    Serial.println("OLED initialized.");
    Serial.println("DS18B20 initialized.");
    Serial.println("Single button initialized.");
    Serial.println("LED indicators initialized.");
    Serial.println("Charging hardware not connected.");

    delay(500);
}


// ---------------- MAIN LOOP --------------------------

void loop() {

    // Handle single-button interaction.

    handleButton();


    // Read temperature asynchronously.

    readTemperature();


    // Refresh OLED.

    updateDisplay();


    // Keep the main loop responsive.

    delay(5);
}