# ============================================================
#  Intelligent Fire Detection System
#  MicroPython Code for ESP8266 NodeMCU
# ============================================================
#  Hardware:
#    - ESP8266 NodeMCU
#    - Flame Sensor → Digital: GPIO5 (D1), Analog: A0
#    - Buzzer       → GPIO4  (D2)
#    - Power        → USB 5V via micro USB
# ============================================================
#  Upload to ESP8266 as main.py via Thonny IDE
#  Required libraries (upload manually):
#    - urequests.py
#    - ujson.py (built-in)
# ============================================================

import machine
import time
import network
import urequests
import ujson
from machine import Pin, ADC

# ─── WiFi Credentials ───────────────────────────────────────
WIFI_SSID     = "YOUR_WIFI_SSID"
WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"

# ─── Blynk Credentials ──────────────────────────────────────
BLYNK_AUTH    = "YOUR_BLYNK_AUTH_TOKEN"
BLYNK_URL     = "https://blynk.cloud/external/api"

# ─── Pin Definitions ────────────────────────────────────────
flame_digital = Pin(5, Pin.IN)    # D1 → GPIO5 (digital output)
buzzer        = Pin(4, Pin.OUT)   # D2 → GPIO4 (buzzer)
flame_analog  = ADC(0)            # A0 → analog intensity
buzzer.value(0)

# ─── State ──────────────────────────────────────────────────
fire_detected   = False
prev_fire_state = False
alert_count     = 0
analog_value    = 0

# ════════════════════════════════════════════════════════════
def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print("[WiFi] Connecting to", WIFI_SSID)
        wlan.connect(WIFI_SSID, WIFI_PASSWORD)
        attempts = 0
        while not wlan.isconnected() and attempts < 20:
            time.sleep(0.5)
            print(".", end="")
            attempts += 1
    if wlan.isconnected():
        print("\n[WiFi] Connected:", wlan.ifconfig()[0])
        beep_buzzer(2, 300)
        return True
    print("\n[WiFi] Failed to connect.")
    return False

# ════════════════════════════════════════════════════════════
def blynk_write(vpin, value):
    """Send value to Blynk virtual pin."""
    try:
        url = "{}/update?token={}&v{}={}".format(
            BLYNK_URL, BLYNK_AUTH, vpin, value)
        r = urequests.get(url)
        r.close()
    except Exception as e:
        print("[Blynk] Write error:", e)

# ════════════════════════════════════════════════════════════
def blynk_notify(message):
    """Send push notification via Blynk."""
    try:
        url  = "https://blynk.cloud/external/api/logEvent"
        data = {
            "token":    BLYNK_AUTH,
            "code":     "fire_alert",
            "description": message
        }
        r = urequests.post(
            url,
            data=ujson.dumps(data),
            headers={"Content-Type": "application/json"}
        )
        r.close()
        print("[Blynk] Notification sent.")
    except Exception as e:
        print("[Blynk] Notify error:", e)

# ════════════════════════════════════════════════════════════
def read_sensors():
    global analog_value, fire_detected
    digital_val  = flame_digital.value()
    analog_value = flame_analog.read()   # 0-1023

    # Active LOW: digital LOW = fire detected
    fire_detected = (digital_val == 0)

    print("[Sensor] Status: {} | Analog: {}".format(
        "FIRE!" if fire_detected else "Safe", analog_value))

# ════════════════════════════════════════════════════════════
def trigger_fire_alert():
    global alert_count
    alert_count += 1
    print("[ALERT] FIRE DETECTED! Alert #{}".format(alert_count))

    buzzer.value(1)

    blynk_write(0, analog_value)   # V0 gauge
    blynk_write(1, 255)            # V1 LED → red

    blynk_notify(
        "FIRE DETECTED! Sensor: {} | Alert #{}".format(
            analog_value, alert_count))

# ════════════════════════════════════════════════════════════
def fire_cleared():
    print("[OK] Fire condition cleared.")
    buzzer.value(0)
    blynk_write(1, 0)   # V1 LED → off (safe)
    blynk_notify("Fire cleared. System monitoring resumed.")

# ════════════════════════════════════════════════════════════
def send_to_cloud():
    blynk_write(0, analog_value)
    blynk_write(1, 255 if fire_detected else 0)

# ════════════════════════════════════════════════════════════
def beep_buzzer(times, delay_ms):
    for _ in range(times):
        buzzer.value(1)
        time.sleep_ms(delay_ms)
        buzzer.value(0)
        time.sleep_ms(delay_ms)

# ════════════════════════════════════════════════════════════
# MAIN
# ════════════════════════════════════════════════════════════
def main():
    global prev_fire_state

    print("\n[BOOT] Intelligent Fire Detection - MicroPython")
    connect_wifi()
    blynk_notify("Fire Detection System Online. Monitoring started.")
    print("[OK] Monitoring started.\n")

    last_sensor  = time.ticks_ms()
    last_cloud   = time.ticks_ms()

    while True:
        now = time.ticks_ms()

        # ── Read sensor every 500ms ────────────────────────
        if time.ticks_diff(now, last_sensor) >= 500:
            last_sensor = now
            read_sensors()

            # Fire just detected
            if fire_detected and not prev_fire_state:
                trigger_fire_alert()

            # Fire cleared
            if not fire_detected and prev_fire_state:
                fire_cleared()

            # Keep buzzer ON while fire persists
            if fire_detected:
                buzzer.value(1)
            else:
                buzzer.value(0)

            prev_fire_state = fire_detected

        # ── Send to cloud every 2 seconds ─────────────────
        if time.ticks_diff(now, last_cloud) >= 2000:
            last_cloud = now
            send_to_cloud()

        time.sleep_ms(100)

# ════════════════════════════════════════════════════════════
main()
