/*
 * ============================================================
 *  Intelligent Fire Detection System
 * ============================================================
 *  Hardware:
 *    - ESP8266 NodeMCU
 *    - Flame Sensor  → Digital: D1 (GPIO5), Analog: A0
 *    - Buzzer        → D2 (GPIO4)
 *    - Power         → USB 5V via micro USB
 * ============================================================
 *  Platform: Blynk IoT (v2.0)
 *  Libraries Required:
 *    - BlynkSimpleEsp8266  (Blynk library)
 *    - ESP8266WiFi         (built-in)
 *    - ESP8266HTTPClient   (built-in)
 * ============================================================
 *  Blynk Virtual Pins:
 *    V0 → Fire sensor analog value (gauge widget)
 *    V1 → Detection status (LED widget: 0=safe, 1=fire)
 *    V2 → Event log (terminal widget)
 * ============================================================
 */

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Fire Detection System"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// ─── WiFi Credentials ───────────────────────────────────────
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// ─── Pin Definitions ────────────────────────────────────────
#define FLAME_DIGITAL_PIN  5    // D1 → GPIO5 (digital output from sensor)
#define BUZZER_PIN         4    // D2 → GPIO4 (buzzer output)
#define FLAME_ANALOG_PIN   A0   // Analog intensity reading

// ─── Thresholds & Timing ────────────────────────────────────
#define SENSOR_INTERVAL_MS    500    // Read sensor every 500ms
#define CLOUD_INTERVAL_MS     2000   // Send to cloud every 2s
#define BUZZER_BEEP_MS        200    // Beep duration

// ─── Blynk Virtual Pins ─────────────────────────────────────
#define VPIN_ANALOG     V0
#define VPIN_STATUS     V1
#define VPIN_LOG        V2

// ─── State Variables ────────────────────────────────────────
bool     fireDetected      = false;
bool     prevFireState     = false;
bool     buzzerOn          = false;
int      analogValue       = 0;
int      alertCount        = 0;

unsigned long lastSensorRead  = 0;
unsigned long lastCloudUpdate = 0;
unsigned long buzzerStart     = 0;

BlynkTimer timer;

// ════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] Intelligent Fire Detection System");

  pinMode(FLAME_DIGITAL_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Startup beep
  beepBuzzer(2, 300);

  // Connect WiFi + Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  // Timer for sensor reading
  timer.setInterval(SENSOR_INTERVAL_MS, readFireSensor);
  // Timer for cloud update
  timer.setInterval(CLOUD_INTERVAL_MS, sendToCloud);

  Serial.println("[OK] System Ready. Monitoring started.");
  Blynk.logEvent("system_online", "Fire Detection System is Online and Monitoring.");
}

// ════════════════════════════════════════════════════════════
void loop() {
  Blynk.run();
  timer.run();

  // Auto stop buzzer after 3 seconds
  if (buzzerOn && (millis() - buzzerStart >= 3000)) {
    if (!fireDetected) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerOn = false;
    }
  }
}

// ════════════════════════════════════════════════════════════
void readFireSensor() {
  int  digitalVal = digitalRead(FLAME_DIGITAL_PIN);
  analogValue     = analogRead(FLAME_ANALOG_PIN);

  // Digital LOW = fire detected (active LOW sensor)
  fireDetected = (digitalVal == LOW);

  Serial.printf("[Sensor] Digital: %s | Analog: %d\n",
    fireDetected ? "FIRE!" : "Safe", analogValue);

  // Fire just detected (rising edge)
  if (fireDetected && !prevFireState) {
    triggerFireAlert();
  }

  // Fire cleared
  if (!fireDetected && prevFireState) {
    fireCleared();
  }

  prevFireState = fireDetected;

  // Keep buzzer ON while fire persists
  if (fireDetected) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerOn    = true;
    buzzerStart = millis();
  }
}

// ════════════════════════════════════════════════════════════
void triggerFireAlert() {
  alertCount++;
  Serial.println("[ALERT] FIRE DETECTED!");

  // Buzzer ON
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerOn    = true;
  buzzerStart = millis();

  // Blynk push notification
  Blynk.logEvent("fire_alert",
    "FIRE DETECTED! Sensor Value: " + String(analogValue) +
    " | Alert #" + String(alertCount));

  // Update Blynk status LED → RED
  Blynk.virtualWrite(VPIN_STATUS, 255);

  // Log to terminal
  Blynk.virtualWrite(VPIN_LOG,
    "FIRE ALERT #" + String(alertCount) +
    " | Sensor: " + String(analogValue) + "\n");
}

// ════════════════════════════════════════════════════════════
void fireCleared() {
  Serial.println("[OK] Fire condition cleared.");
  digitalWrite(BUZZER_PIN, LOW);
  buzzerOn = false;

  // Update Blynk status LED → GREEN (safe)
  Blynk.virtualWrite(VPIN_STATUS, 0);

  // Log to terminal
  Blynk.virtualWrite(VPIN_LOG, "All Clear. System back to monitoring.\n");

  Blynk.logEvent("fire_cleared", "Fire condition cleared. System monitoring resumed.");
}

// ════════════════════════════════════════════════════════════
void sendToCloud() {
  // Send analog sensor value to gauge widget
  Blynk.virtualWrite(VPIN_ANALOG, analogValue);

  // Send detection status
  Blynk.virtualWrite(VPIN_STATUS, fireDetected ? 255 : 0);
}

// ════════════════════════════════════════════════════════════
void beepBuzzer(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(delayMs);
    digitalWrite(BUZZER_PIN, LOW);
    delay(delayMs);
  }
}
