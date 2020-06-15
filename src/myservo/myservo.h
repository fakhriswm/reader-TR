#ifndef MYSERVO_H_
  #define MYSERVO_H_

#include <Arduino.h>
#include <Servo.h>

#define POS_UP 90
#define POS_DOWN 0

class myservo{
  public:
    void myservo_init(const int servoPin);
    void myservo_open();
    void myservo_close();
};


#endif

