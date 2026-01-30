#include <IRremote.h>
int RECV_PIN = A0;
IRrecv irrecv(RECV_PIN);

decode_results results;

void setup(){
Serial.begin(9600);
irrecv.enableIRIn();
}

void loop() {
if (irrecv.decode(&results)) {

  if(results.value == 16753245)
  
  {
    Serial.println("asdfasdfasdf adf asdf asdfasdf");
    }
Serial.println(results.value);
delay(100);
irrecv.resume();
}
}
