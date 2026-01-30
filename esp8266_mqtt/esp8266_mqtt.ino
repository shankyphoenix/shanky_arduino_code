#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID     = "OnePlus 11R 5G";
const char* WIFI_PASSWORD = "ryanShank07";

const char* MQTT_HOST     = "host4.phoenixwork.link";
const int   MQTT_PORT     = 1883;

const char* MQTT_USER     = "hauser";
const char* MQTT_PASSWORD = "jiShanky27";

const char* DEVICE_ID     = "esp8266_1";

WiFiClient espClient;
PubSubClient mqtt(espClient);

String tStatus = String("home/") + DEVICE_ID + "/status";
String tSensor = String("home/") + DEVICE_ID + "/sensor";
String tState  = String("home/") + DEVICE_ID + "/state";
String tCmd    = String("home/") + DEVICE_ID + "/cmd";

const int OUTPUT_PIN = LED_BUILTIN; // change if relay

void setOutput(bool on) {
  digitalWrite(OUTPUT_PIN, on ? LOW : HIGH); // LED_BUILTIN active LOW
}
bool getOutput() { return digitalRead(OUTPUT_PIN) == LOW; }

void onMsg(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  Serial.print("MQTT IN  "); Serial.print(topic);
  Serial.print(" = "); Serial.println(msg);

  if (String(topic) == tCmd) {
    if (msg.equalsIgnoreCase("ON")) setOutput(true);
    else if (msg.equalsIgnoreCase("OFF")) setOutput(false);
    else if (msg.equalsIgnoreCase("TOGGLE")) setOutput(!getOutput());

    mqtt.publish(tState.c_str(), getOutput() ? "ON" : "OFF", true);
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi OK. IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMsg);

  while (!mqtt.connected()) {
    String clientId = String(DEVICE_ID) + "-" + String(ESP.getChipId(), HEX);
    Serial.print("MQTT connecting as "); Serial.println(clientId);

    bool ok = mqtt.connect(
      clientId.c_str(),
      MQTT_USER, MQTT_PASSWORD,
      tStatus.c_str(), 1, true, "offline"
    );

    if (ok) {
      Serial.println("MQTT OK");
      mqtt.publish(tStatus.c_str(), "online", true);
      mqtt.subscribe(tCmd.c_str(), 1);
      mqtt.publish(tState.c_str(), getOutput() ? "ON" : "OFF", true);
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("BOOT");

  pinMode(OUTPUT_PIN, OUTPUT);
  setOutput(false);

  connectWiFi();
  connectMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMQTT();

  mqtt.loop();

  static unsigned long last = 0;
  if (millis() - last > 10000) {
    last = millis();
    long rssi = WiFi.RSSI();
    char payload[16];
    snprintf(payload, sizeof(payload), "%ld", rssi);
    mqtt.publish(tSensor.c_str(), payload, true);
    Serial.print("PUB RSSI "); Serial.println(payload);
  }
}
