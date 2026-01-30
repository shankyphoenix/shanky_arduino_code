#include <ESP8266WiFi.h>

//const char* ssid = "YOUR_WIFI_NAME";
//const char* password = "YOUR_WIFI_PASSWORD";
const char* ssid = "OnePlus 11R 5G";
const char* password = "ryanShank07";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED OFF (active LOW)

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New Client");
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>NodeMCU Web Server</h1>");
  client.println("<a href=\"/LED=ON\"><button>LED ON</button></a>");
  client.println("<a href=\"/LED=OFF\"><button>LED OFF</button></a>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disconnected");
}
