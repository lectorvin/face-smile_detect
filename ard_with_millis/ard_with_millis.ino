#include <Servo.h>
Servo servo1;
Servo servo2;
Servo servo3;
int x=0, y=0, z=0;
int tmp, tmp2;
int vertical = 10;
unsigned long currentMillis=0, previousMillis=0, previousMillis2=0;
int key = 10;
int j = 2000;
int keyAction = -1;
unsigned long interval = 99999;

void setup() {
    servo1.attach(12);
    servo2.attach(13);
    servo3.attach(2);
    Serial.begin(9600);
    servo1.writeMicroseconds(1500);
    servo2.write(vertical);
    servo3.writeMicroseconds(1500);
}

void sayHello(int u) {
    servo3.writeMicroseconds(u);
    delay(10);
    servo3.writeMicroseconds(1500);
    delay(10);
}


void loop() {
    currentMillis = millis();
    if ((currentMillis-previousMillis) > 3000) {  
      key *= -1;
      previousMillis = currentMillis;  
    }
    if ((currentMillis-previousMillis2) > interval) {  
      keyAction = -1;
      interval = 99999;
      previousMillis2 = currentMillis; 
    }
    
    if(Serial.available()) {
      tmp = Serial.read();
      x = map(tmp, 0, 255, 1200, 1720);
      delay(10);
      y = Serial.read();
      delay(10);
      z = Serial.read();
  
      servo1.writeMicroseconds((int)x);
      delay(20);
      servo1.writeMicroseconds(1500);
      delay(10);
      servo2.write((int)y);
 
      if (z==1) {
         interval = 5000;
         keyAction = 1; 
      }
    }
    if (keyAction==1) {
      if (key==10)
        sayHello(2500);
      else  
        sayHello(0);   
    }
}
