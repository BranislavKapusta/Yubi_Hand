#pragma once
#include <ESP32Servo.h>

#define FINGER_NUM 6

//### Class ##########################################
class Finger {
public:
    int pin;
    int upperLimit;
    int lowerLimit;
    int defaultPos;

    //State
    bool servoOn;

    int currentPos;
    int targetPos;
    float speed;

    unsigned long lastUpdateTime;
    float stepAccumulator;

    void init(int _pin, int _lower, int _upper, int _default, float _speed = 1);
    void turnOn();
    void turnOff();
    void setPos(float angleDeg);
    void setDuration(int speed);
    void move(int value);
    void step();
    bool posCheck();

private:
    Servo _finger;
    int _convertAngles(float angleDeg);
    void enable();
    void disable();
};

//### Variables and Constants ##########################################
extern Finger fingers[FINGER_NUM];

//### Functions ########################################################
void init_finger_motors_setup_v2();