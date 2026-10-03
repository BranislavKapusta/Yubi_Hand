#include "finger_v2.h"
#include <ESP32Servo.h>

//### Variables and Constants ##########################################
static const int fingers_pins[FINGER_NUM] = {25,26,27,14,33,13};

static const int fingers_default_pos[FINGER_NUM] = {0,0,0,0,0,0};
static const int fingers_upperLimits_pos[FINGER_NUM] = {2000,2000,2000,2000,2000,2000};
static const int fingers_lowerLimits_pos[FINGER_NUM] = {1100,1100,1100,1100,1100,1200};

static const int fingers_commands_pos[2] = {0,90};

Finger fingers[FINGER_NUM];

//### Class finger functions ###########################################
void Finger::init(int _pin, int _lower, int _upper, int _default, float _speed){
    pin = _pin;
    lowerLimit = _lower;
    upperLimit = _upper;
    defaultPos = _default;
    speed = _speed;

    lastUpdateTime = millis();
    stepAccumulator = 0.0f;

    currentPos = _convertAngles(10);
    targetPos = currentPos;

    servoOn = false;

    turnOn();
}

void Finger::enable(){
    _finger.setPeriodHertz(50);
    _finger.attach(pin, 1000, 2000);
};

void Finger::disable(){
    _finger.detach();
};

void Finger::turnOn(){
    if(!servoOn){
        enable();
        servoOn = true;
    }
};

void Finger::turnOff(){
    if(servoOn){
        disable();
        servoOn = false;
    }
}

void Finger::setPos(float angleDeg){
    targetPos = _convertAngles(angleDeg);
    lastUpdateTime = millis();
    stepAccumulator = 0.0f;
}

void Finger::setDuration(int input_speed){
    int constrained = constrain(input_speed, 1, 1000);
    speed = (float)constrained/1000;
    Serial.printf("Speed set: %f\n",speed);
}

void Finger::move(int value){
    _finger.writeMicroseconds(value);
}

void Finger::step(){
    if(currentPos == targetPos) return;
    
    stepAccumulator += speed;

    int step = (int)stepAccumulator;
    if(step == 0) return;
    stepAccumulator -= step;
    
    if(currentPos > targetPos){
        currentPos = max(currentPos - step, targetPos);
    }else{
        currentPos = min(currentPos + step, targetPos);
    }

    move(currentPos);
}

bool Finger::posCheck(){
    return currentPos == targetPos;
}

int Finger::_convertAngles(float angleDeg){
    return map(constrain(angleDeg, fingers_commands_pos[0], fingers_commands_pos[1]), fingers_commands_pos[0], fingers_commands_pos[1], upperLimit, lowerLimit);
}

//### Functions ########################################################
void init_finger_motors_setup_v2(){
    for(int i=0; i<FINGER_NUM; i++){
        fingers[i].init(fingers_pins[i], fingers_lowerLimits_pos[i],fingers_upperLimits_pos[i],fingers_default_pos[i],(float)1000);
    }
}