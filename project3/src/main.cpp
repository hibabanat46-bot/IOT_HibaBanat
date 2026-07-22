#include <Arduino.h>
// ------------------------- PIN DEFINITIONS -------------------------
const int PIN_CURRENT_SENSOR = 34;   // ACS712 analog output (ADC1 input, input-only pin)
const int PIN_RELAY          = 26;   // Relay control (Active LOW)
const int PIN_BUTTON         = 27;   // Emergency stop / start-stop button (INPUT_PULLUP)

const int PIN_LED_GREEN      = 16;   // Motor running
const int PIN_LED_YELLOW     = 17;   // Motor stopped
const int PIN_LED_BLUE       = 18;   // Low current
const int PIN_LED_RED        = 19;   // High current (fault)

// ------------------------- RELAY / LOGIC LEVELS ---------------------
const int RELAY_ON  = LOW;           // Active LOW relay module
const int RELAY_OFF = HIGH;

// ------------------------- ADC / SENSOR CONSTANTS --------------------
const float ADC_VREF                = 3.3f;     // ESP32 ADC reference voltage
const int   ADC_RESOLUTION          = 4095;      // 12-bit ADC (0-4095)
const float VOLTAGE_DIVIDER_RATIO   = 1.0f;      // Set > 1.0 if using a divider (see note above)
const float ACS712_SENSITIVITY      = 0.066f;    // V per A, ACS712-30A
const float NOISE_FLOOR_CURRENT     = 0.03f;     // Ignore current readings below this (A)

const int CALIBRATION_SAMPLES = 800;   // Number of samples used at startup (500-1000 range)
const int CURRENT_SAMPLES     = 200;   // Samples averaged per readCurrent() call (raised for a noisy ACS712 signal)


const float CURRENT_EMA_ALPHA = 0.2f;

const unsigned long OVERCURRENT_CONFIRM_MS = 150;
const unsigned long STARTUP_GRACE_MS = 500;

// ------------------------- PROTECTION THRESHOLDS ----------------------
const float HIGH_CURRENT_THRESHOLD = .50f;   // Amps - trips protection immediately
const float LOW_CURRENT_THRESHOLD  = 0.05f;  // Amps - below this, Blue LED indicates low current

// ------------------------- TIMING CONSTANTS ---------------------------
const unsigned long DEBOUNCE_DELAY_MS   = 50;    // Button software debounce
const unsigned long AUTO_RESTART_DELAY_MS = 3000; // Wait time after fault clears before restart
const unsigned long PRINT_INTERVAL_MS   = 200;   // Serial status print interval

// ------------------------- SYSTEM STATE MACHINE -----------------------
enum SystemState {
  STATE_STOPPED,        // Motor off, idle (default / after emergency stop released back to idle)
  STATE_RUNNING,        // Motor on, current within normal range
  STATE_LOW_CURRENT,    // Motor on, but current below LOW_CURRENT_THRESHOLD
  STATE_HIGH_CURRENT,   // Fault: overcurrent detected, motor forcibly disabled
  STATE_EMERGENCY_STOP  // Motor stopped by user via the push button
};

SystemState systemState = STATE_STOPPED;

// ------------------------- GLOBAL RUNTIME VARIABLES --------------------
float zeroVoltage   = 0.0f;   // Calibrated sensor offset voltage (no-load output)
float lastCurrent   = 0.0f;   // Most recent filtered current reading (A)
float lastVoltage   = 0.0f;   // Most recent raw sensor voltage (V)

bool motorRunning = false;    // True when relay is energized / motor commanded ON
unsigned long motorStartTime = 0; // millis() timestamp of the last startMotor() call

// Button debounce state
int  lastRawButtonReading   = HIGH;
int  stableButtonState      = HIGH;
unsigned long lastDebounceTime = 0;

// High current trip / auto-restart timing
unsigned long highCurrentClearedTime = 0; // millis() timestamp when current became safe again
bool waitingForRestart = false;

// Overcurrent confirmation timing (prevents false trips from ADC noise)
unsigned long overCurrentStartTime = 0;
bool overCurrentTimerRunning = false;

// Exponential moving average state for the current reading
float smoothedCurrent = 0.0f;
bool  smoothedCurrentInitialized = false;

// Serial print timing
unsigned long lastPrintTime = 0;

// ------------------------- FUNCTION PROTOTYPES -------------------------
// Required because PlatformIO (main.cpp) does not auto-generate these

void setupPins();
void calibrateSensor();
void readCurrent();
void startMotor();
void stopMotor();
void handleButton();
void onButtonPressed();
void protectionLogic();
void updateLEDs();
void printStatus();
const char* stateToString(SystemState state);


void setup() {
  Serial.begin(115200);
  delay(200);

  setupPins();
  stopMotor();          // Ensure motor/relay start in a known OFF state
  calibrateSensor();    // Establish zero-current reference voltage

  systemState = STATE_STOPPED;
  updateLEDs();

  Serial.println(F("==================================================="));
  Serial.println(F(" ESP32 DC Motor Protection System - Initialized"));
  Serial.println(F("==================================================="));
}


void loop() {
  handleButton();       // Check for user start/stop requests
  readCurrent();         // Update lastCurrent / lastVoltage
  protectionLogic();     // Apply high/low current protection rules
  updateLEDs();           // Reflect current state on the LEDs
  printStatus();          // Periodic serial reporting
}


void setupPins() {
  pinMode(PIN_CURRENT_SENSOR, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  digitalWrite(PIN_RELAY, RELAY_OFF);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_BLUE, LOW);
  digitalWrite(PIN_LED_RED, LOW);
}


void calibrateSensor() {
  Serial.println(F("Calibrating current sensor... keep motor OFF."));

  double total = 0.0;

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    total += analogRead(PIN_CURRENT_SENSOR);
    delayMicroseconds(500); // Small spacing between samples for ADC stability
  }

  float averageRaw = total / CALIBRATION_SAMPLES;
  zeroVoltage = (averageRaw / ADC_RESOLUTION) * ADC_VREF * VOLTAGE_DIVIDER_RATIO;

  Serial.print(F("Calibration complete. Zero-current voltage = "));
  Serial.print(zeroVoltage, 4);
  Serial.println(F(" V"));
}


void readCurrent() {
  long total = 0;

  for (int i = 0; i < CURRENT_SAMPLES; i++) {
    total += analogRead(PIN_CURRENT_SENSOR);
  }

  float averageRaw = (float)total / CURRENT_SAMPLES;
  float sensorVoltage = (averageRaw / ADC_RESOLUTION) * ADC_VREF * VOLTAGE_DIVIDER_RATIO;

  float current = fabs(sensorVoltage - zeroVoltage) / ACS712_SENSITIVITY;

  // Ignore very small values to reduce noise from ADC jitter
  if (current < NOISE_FLOOR_CURRENT) {
    current = 0.0f;
  }

 
  if (!smoothedCurrentInitialized) {
    smoothedCurrent = current;
    smoothedCurrentInitialized = true;
  } else {
    smoothedCurrent = (CURRENT_EMA_ALPHA * current) + ((1.0f - CURRENT_EMA_ALPHA) * smoothedCurrent);
  }

  lastVoltage = sensorVoltage;
  lastCurrent = smoothedCurrent;
}


void startMotor() {
  digitalWrite(PIN_RELAY, RELAY_ON);
  motorRunning = true;
  motorStartTime = millis();
}


void stopMotor() {
  digitalWrite(PIN_RELAY, RELAY_OFF);
  motorRunning = false;
}


void handleButton() {
  int rawReading = digitalRead(PIN_BUTTON);

  // Reset debounce timer whenever the raw reading changes
  if (rawReading != lastRawButtonReading) {
    lastDebounceTime = millis();
  }

  // If the reading has been stable longer than the debounce delay,
  // accept it as the new stable state
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
    if (rawReading != stableButtonState) {
      stableButtonState = rawReading;

      // Falling edge = button just pressed (pulled up, pressed = LOW)
      if (stableButtonState == LOW) {
        onButtonPressed();
      }
    }
  }

  lastRawButtonReading = rawReading;
}

// Handles a single confirmed button press event
void onButtonPressed() {
  bool motorCurrentlyActive =
      (systemState == STATE_RUNNING || systemState == STATE_LOW_CURRENT);

  if (motorCurrentlyActive) {
    stopMotor();
    systemState = STATE_EMERGENCY_STOP;
    waitingForRestart = false; // Cancel any pending auto-restart
    Serial.println(F(">>> Emergency Stop button pressed: motor stopped."));
  } else if (systemState == STATE_STOPPED || systemState == STATE_EMERGENCY_STOP) {
    startMotor();
    systemState = STATE_RUNNING;
    Serial.println(F(">>> Start button pressed: motor running."));
  }

}


void protectionLogic() {
  bool withinStartupGrace = motorRunning && (millis() - motorStartTime < STARTUP_GRACE_MS);

  // ---- HIGH CURRENT FAULT (confirmed over time, not a single sample) ----
  if (!withinStartupGrace && motorRunning && lastCurrent > HIGH_CURRENT_THRESHOLD) {
    if (!overCurrentTimerRunning) {
      overCurrentTimerRunning = true;
      overCurrentStartTime = millis();
    } else if (millis() - overCurrentStartTime >= OVERCURRENT_CONFIRM_MS) {
      stopMotor();
      systemState = STATE_HIGH_CURRENT;
      waitingForRestart = false;
      overCurrentTimerRunning = false;
      Serial.print(F("!!! WARNING: High current detected ("));
      Serial.print(lastCurrent, 3);
      Serial.println(F(" A). Motor disabled for protection."));
    }
    return;
  } else {
   
    overCurrentTimerRunning = false;
  }

  // ---- MANAGE AUTO-RESTART AFTER A TRIP ----
  if (systemState == STATE_HIGH_CURRENT) {
    if (lastCurrent <= HIGH_CURRENT_THRESHOLD) {
      if (!waitingForRestart) {
        waitingForRestart = true;
        highCurrentClearedTime = millis();
        Serial.println(F("Current back to safe levels. Waiting to auto-restart..."));
      } else if (millis() - highCurrentClearedTime >= AUTO_RESTART_DELAY_MS) {
        startMotor();
        systemState = STATE_RUNNING;
        waitingForRestart = false;
        Serial.println(F(">>> Auto-restart complete. Motor running."));
      }
    } else {
      // Current spiked again while waiting; reset the restart timer
      waitingForRestart = false;
    }
    return; // Do not evaluate low-current logic while tripped
  }

  // ---- LOW CURRENT DETECTION (motor keeps running) ----
  if (motorRunning) {
    if (lastCurrent < LOW_CURRENT_THRESHOLD) {
      systemState = STATE_LOW_CURRENT;
    } else {
      systemState = STATE_RUNNING;
    }
  }
}


void updateLEDs() {
  bool motorIsOn = (systemState == STATE_RUNNING || systemState == STATE_LOW_CURRENT);

  digitalWrite(PIN_LED_GREEN,  motorIsOn ? HIGH : LOW);
  digitalWrite(PIN_LED_YELLOW, motorIsOn ? LOW  : HIGH);

  digitalWrite(PIN_LED_BLUE, (systemState == STATE_LOW_CURRENT)  ? HIGH : LOW);
  digitalWrite(PIN_LED_RED,  (systemState == STATE_HIGH_CURRENT) ? HIGH : LOW);
}

void printStatus() {
  unsigned long now = millis();
  if (now - lastPrintTime < PRINT_INTERVAL_MS) {
    return;
  }
  lastPrintTime = now;

  Serial.print(F("Current: "));
  Serial.print(lastCurrent, 3);
  Serial.print(F(" A | Voltage: "));
  Serial.print(lastVoltage, 3);
  Serial.print(F(" V | Motor: "));
  Serial.print(motorRunning ? F("RUNNING") : F("STOPPED"));
  Serial.print(F(" | Relay: "));
  Serial.print(motorRunning ? F("ON") : F("OFF"));
  Serial.print(F(" | Protection: "));
  Serial.println(stateToString(systemState));
}

const char* stateToString(SystemState state) {
  switch (state) {
    case STATE_STOPPED:        return "STOPPED";
    case STATE_RUNNING:        return "NORMAL";
    case STATE_LOW_CURRENT:    return "LOW_CURRENT";
    case STATE_HIGH_CURRENT:   return "HIGH_CURRENT_TRIP";
    case STATE_EMERGENCY_STOP: return "EMERGENCY_STOP";
    default:                   return "UNKNOWN";
  }
}
