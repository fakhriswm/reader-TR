#include "myservo/myservo.h"

Servo servo1;

void myservo :: myservo_init(const int servoPin){
  servo1.attach(servoPin);
}

void myservo :: myservo_open(){
  servo1.write(POS_UP);
  delay(100);
}

void myservo :: myservo_close(){
  servo1.write(POS_DOWN);
  delay(100);
}