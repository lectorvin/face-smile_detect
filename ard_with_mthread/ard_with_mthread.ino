#include <Servo.h>
#include <mthread.h>

Servo servo1;
Servo servo2;
Servo servo3;
int x, y, z;
int tmp, tmp2;
int vertical = 10;

class SayHelloEvent : public EventHandler {
  public:
    SayHelloEvent();
  protected:
    bool condition();
    bool on_event();  
};
SayHelloEvent::SayHelloEvent() {}
bool SayHelloEvent::condition() {
  if (z==1) { return true; }
  return false;
}
bool SayHelloEvent::on_event() {
  for(int i=1300;i<1720;i=i+20) {
    servo3.writeMicroseconds(i);
    delay(100);
  }
  servo3.writeMicroseconds(1500);
  delay(10); 
}


class MyThread : public Thread {
  public:
    MyThread();
  protected:
    bool loop();
};
MyThread::MyThread() {}
bool MyThread::loop() {
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
  return true;
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
    ThreadList tl = new ThreadList();
    tl.add_thread(new SayHelloEvent);
    tl.add_thread(new MyThread);
    main_thread_list->add_thread(tl);
}

