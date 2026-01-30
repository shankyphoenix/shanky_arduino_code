/****************************************************
 * ESP First-Time Setup + JSN-SR04T Distance + POST distance
 *
 * Flow:
 * 1) If no WiFi saved (or can't connect) -> AP + captive portal (WiFiManager)
 * 2) User enters WiFi SSID/password + App Username + App Password
 * 3) App creds saved to SPIFFS (/config.json)
 * 4) Device connects to WiFi
 * 5) Reads JSN-SR04T distance (cm)
 * 6) POSTs "distance" to:
 *    http://host4.phoenixwork.link:8080/info.php
 * 7) Repeats every 10 seconds
 *
 * Board: NodeMCU (ESP8266 ESP-12E)
 *
 * Libraries:
 * - WiFiManager by tzapu
 * - ArduinoJson (v6+)
 ****************************************************/

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <FS.h>              // SPIFFS
#else
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include "SPIFFS.h"
#endif

#include <WiFiManager.h>
#include <ArduinoJson.h>

// ---------- Portal / Config ----------
static const char* AP_SSID     = "ESP-Setup";
static const char* AP_PASS     = "CamSetup123";     // min 8 chars
static const char* CONFIG_PATH = "/config.json";

// ---------- POST endpoint ----------
static const char* DIST_URL    = "http://host4.phoenixwork.link:8080/info.php";

// ---------- NodeMCU pins for JSN-SR04T ----------
static const int TRIG_PIN = 5; // D1 (GPIO5)
static const int ECHO_PIN = 4; // D2 (GPIO4)  <-- MUST be via voltage divider

// ---------- Sensor timing ----------
static const unsigned long PULSE_TIMEOUT_US = 30000UL; // 30ms ~ up to ~5m

// ---------- Globals ----------
String appUser;
String appPass;

// ---------- FS helpers ----------
bool fsBegin() {
#if defined(ESP8266)
  return SPIFFS.begin();
#else
  return SPIFFS.begin(true); // auto-format if needed
#endif
}

bool loadConfig() {
  if (!fsBegin()) {
    Serial.println("SPIFFS begin failed");
    return false;
  }
  if (!SPIFFS.exists(CONFIG_PATH)) {
    Serial.println("No config file yet");
    return false;
  }

  File f = SPIFFS.open(CONFIG_PATH, "r");
  if (!f) {
    Serial.println("Failed to open config file");
    return false;
  }

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    Serial.print("deserializeJson failed: ");
    Serial.println(err.c_str());
    return false;
  }

  if (doc.containsKey("appUser")) appUser = doc["appUser"].as<const char*>();
  else appUser = "";

  if (doc.containsKey("appPass")) appPass = doc["appPass"].as<const char*>();
  else appPass = "";

  Serial.println("Loaded config from SPIFFS");
  return true;
}

bool saveConfig(const String& u, const String& p) {
  if (!fsBegin()) {
    Serial.println("SPIFFS begin failed");
    return false;
  }

  StaticJsonDocument<256> doc;
  doc["appUser"] = u;
  doc["appPass"] = p;

  File f = SPIFFS.open(CONFIG_PATH, "w");
  if (!f) {
    Serial.println("Failed to open config for writing");
    return false;
  }

  if (serializeJson(doc, f) == 0) {
    Serial.println("serializeJson failed");
    f.close();
    return false;
  }
  f.close();

  Serial.println("Saved config to SPIFFS");
  return true;
}

// ---------- JSN-SR04T distance reading ----------
static inline float usToCm(unsigned long us) {
  // speed of sound ~343 m/s => 0.0343 cm/us
  // divide by 2 because round-trip
  return (us * 0.0343f) / 2.0f;
}

// Returns distance in cm, or -1 if failed/timeout
float readDistanceCm() {
  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Echo pulse width
  unsigned long echoUs = pulseIn(ECHO_PIN, HIGH, PULSE_TIMEOUT_US);
  if (echoUs == 0) return -1.0f; // timeout (no echo)

  float cm = usToCm(echoUs);

  // sanity range for JSN-SR04T
  if (cm < 2.0f || cm > 600.0f) return -1.0f;

  return cm;
}

// ---------- HTTP POST distance ----------
bool postDistance(float distance) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }

  String url = String(DIST_URL);

#if defined(ESP8266)
  WiFiClient client;
  HTTPClient http;
  if (!http.begin(client, url)) {
    Serial.println("http.begin failed");
    return false;
  }
#else
  HTTPClient http;
  if (!http.begin(url)) {
    Serial.println("http.begin failed");
    return false;
  }
#endif

  http.setTimeout(8000);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // POST body: distance=<value>
  String body = "distance=" + String(distance, 2);

  Serial.println("POST -> " + url);
  Serial.println("Body -> " + body);

  int code = http.POST(body);
  String resp = http.getString();
  http.end();

  Serial.print("HTTP code: ");
  Serial.println(code);
  Serial.println("Response:");
  Serial.println(resp);

  return (code > 0 && code < 400);
}

// ---------- Optional: reset everything ----------
void factoryReset() {
  Serial.println("Factory reset...");
  WiFiManager wm;
  wm.resetSettings(); // clears WiFi creds
  if (fsBegin()) SPIFFS.remove(CONFIG_PATH); // clears app user/pass
  delay(1000);
#if defined(ESP8266)
  ESP.restart();
#else
  ESP.restart();
#endif
}

void setup() {
  Serial.begin(115200);
  delay(300);

  // Sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Load saved app creds (if present)
  loadConfig();

  WiFiManager wm;

  // Add extra fields to captive portal
  WiFiManagerParameter p_user("app_user", "App Username", appUser.c_str(), 32);
  WiFiManagerParameter p_pass("app_pass", "App Password", appPass.c_str(), 64);

  wm.addParameter(&p_user);
  wm.addParameter(&p_pass);

  // Portal behavior
  wm.setConfigPortalTimeout(180); // seconds

  // Uncomment to force AP every boot (testing)
  // wm.resetSettings();

  // Start portal only if needed; otherwise connects with saved WiFi
  bool ok = wm.autoConnect(AP_SSID, AP_PASS);

  if (!ok) {
    Serial.println("WiFiManager failed or timed out -> restarting");
    delay(1000);
#if defined(ESP8266)
    ESP.restart();
#else
    ESP.restart();
#endif
  }

  // Read portal values
  appUser = String(p_user.getValue());
  appPass = String(p_pass.getValue());

  // Save your custom creds
  saveConfig(appUser, appPass);

  Serial.println("WiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // First reading + POST
  float distance = readDistanceCm();
  if (distance < 0) {
    Serial.println("Distance read failed (check JSN power + ECHO divider + wiring)");
  } else {
    Serial.print("Distance cm: ");
    Serial.println(distance, 2);

    bool sent = postDistance(distance);
    Serial.println(sent ? "Distance POST OK" : "Distance POST FAILED");
  }
}

void loop() {
  // Post every 10 seconds
  static unsigned long last = 0;
  if (millis() - last > 10000) {
    last = millis();

    float distance = readDistanceCm();
    if (distance >= 0) {
      Serial.print("Distance cm: ");
      Serial.println(distance, 2);
      postDistance(distance);
    } else {
      Serial.println("Distance read failed");
    }
  }
}
