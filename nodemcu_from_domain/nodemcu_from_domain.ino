#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define LED                             13
const char* ssid = "shanky";
const char* password = "Shanky123!";
void setup () {

  pinMode(LED, OUTPUT);
//digitalWrite(LED, HIGH);
 
Serial.begin(115200);
WiFi.begin(ssid, password);
 
while (WiFi.status() != WL_CONNECTED) {
 
delay(1000);
Serial.print("Connecting..");
 
}
 
}
 
void loop() {
    
 
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

   Serial.println("connected"); 
   
  HTTPClient http;  //Declare an object of class HTTPClient
   
  http.begin("https://norgren-interlynx.com/status.php","EA:30:90:54:DA:44:E2:B3:09:D9:23:CD:8B:B8:12:14:42:AA:EE:31");  //Specify request destination
  int httpCode = http.GET();                                                                  //Send the request
  Serial.println(httpCode);
   
  if (httpCode > 0) { //Check the returning code
   
    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);                     //Print the response payload
    
    


if(payload == "on"){
   digitalWrite(LED, HIGH);
}
else
{
     digitalWrite(LED, LOW);
}

   
  }
   
  http.end();   //Close connection
   
  }
 
  delay(4000);    //Send a request every 30 seconds
 
}
