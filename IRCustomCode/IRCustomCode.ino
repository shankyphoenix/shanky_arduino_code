#include <IRremote.h>
int RECV_PIN = A0;
boolean toggle5 = false;
boolean toggle6 = false;
boolean toggle7 = false;
boolean toggle8 = false;
boolean toggle9 = false;
boolean toggle10 = false;
boolean toggle11 = false;
boolean toggle12 = false;
boolean toggle13 = false;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup(){
Serial.begin(9600);
irrecv.enableIRIn();
pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);
pinMode(9, OUTPUT);
pinMode(10, OUTPUT);
pinMode(11, OUTPUT);
pinMode(12, OUTPUT);
pinMode(13, OUTPUT);
}

void loop() {
if (irrecv.decode(&results)) {
if(results.value == 16753245){  toggle5 = !toggle5; digitalWrite(5, toggle5);  }
else if(results.value == 16736925){  toggle6 = !toggle6; digitalWrite(6, toggle6);  }
else if(results.value == 16769565){  toggle7 = !toggle7; digitalWrite(7, toggle7);  }
else if(results.value == 16720605){  toggle8 = !toggle8; digitalWrite(8, toggle8);  }
else if(results.value == 16712445){  toggle9 = !toggle9; digitalWrite(9, toggle9);  }
else if(results.value == 16761405){  toggle10 = !toggle10; digitalWrite(10, toggle10);  }
else if(results.value == 16769055){  toggle11 = !toggle11; digitalWrite(11, toggle11);  }
else if(results.value == 16754775){  toggle12 = !toggle12; digitalWrite(12, toggle12);  }
else if(results.value == 16748655){  toggle13 = !toggle13; digitalWrite(13, toggle13);  }
else if(results.value == 16738455){  
  toggle5 = 0;
  toggle6 = 0;
  toggle7 = 0;
  toggle8 = 0;
  toggle9 = 0;
  toggle10 = 0;
  toggle11 = 0;
  toggle12 = 0;
  toggle13 = 0;
  digitalWrite(5, toggle5);
  digitalWrite(6, toggle6);
  digitalWrite(7, toggle7);
  digitalWrite(8, toggle8);
  digitalWrite(9, toggle9);
  digitalWrite(10, toggle10);
  digitalWrite(11, toggle11);
  digitalWrite(12, toggle12);
  digitalWrite(13, toggle13);
 }
else if(results.value == 16756815){  
  toggle5 = 1;
  toggle6 = 1;
  toggle7 = 1;
  toggle8 = 1;
  toggle9 = 1;
  toggle10 = 1;
  toggle11 = 1;
  toggle12 = 1;
  toggle13 = 1;
  digitalWrite(5, toggle5);
  digitalWrite(6, toggle6);
  digitalWrite(7, toggle7);
  digitalWrite(8, toggle8);
  digitalWrite(9, toggle9);
  digitalWrite(10, toggle10);
  digitalWrite(11, toggle11);
  digitalWrite(12, toggle12);
  digitalWrite(13, toggle13);
  }
Serial.println(results.value);
delay(100);
irrecv.resume();
}
}
