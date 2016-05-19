#include <Thread.h>
#include <ThreadController.h>
#include <Servo.h>

ThreadController controll = ThreadController();

Thread sayHelloThread = Thread();
Thread mainThread = Thread();

Servo servo1;
Servo servo2;
Servo servo3;
int x, y, z;
int tmp, tmp2;
int vertical = 120;

void sayHelloCallback() {
   if (z==1) {
     for(int i=1300;i<1720;i=i+20) {
       servo3.writeMicroseconds(i);
       delay(100);
     }
     servo3.writeMicroseconds(1500);
     delay(10);
   }
}

void mainCallback() {
  while (Serial.available()) {
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

    Serial.print(x);
    Serial.print(' ');
    Serial.print(y);
    Serial.print(' ');
    Serial.println(z);
  }
}

void setup() {
  servo1.attach(12);
  servo2.attach(13);
  servo3.attach(2);
  Serial.begin(9600);
  servo1.writeMicroseconds(1500);
  servo2.write(vertical);
  delay(50);
  servo3.writeMicroseconds(1500);
    
  sayHelloThread.onRun(sayHelloCallback);
  mainThread.onRun(mainCallback);
   
  controll.add(&sayHelloThread);
  controll.add(&mainThread);
}

void loop() {
  controll.run(); 
}
