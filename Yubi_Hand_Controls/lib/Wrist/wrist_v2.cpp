#include "wrist_v2.h"
#include <ESP32Servo.h>

//### Variables and Constants ##########################################
static const int wrists_pins[WRIST_NUM] = {23,22};
static const int wrists_default_pos[WRIST_NUM] = {89,89};
static const int wrists_upperLimits_pos[WRIST_NUM] = {1925,1925};
static const int wrists_lowerLimits_pos[WRIST_NUM] = {975,975};

static const int wrists_commands_pos[2] = {55,125};

Wrist wrists[WRIST_NUM];

//### Class wrist functions ###########################################
void Wrist::init(int _pin, int _lower, int _upper, int _default, float _speed){
    pin = _pin;
    lowerLimit = _lower;
    upperLimit = _upper;
    defaultPos = _default;
    speed = _speed;

    lastUpdateTime = millis();
    stepAccumulator = 0.0f;

    currentPos = _convertAngles(87);
    targetPos = currentPos;

    servoOn = false;

    turnOn();
}

void Wrist::enable(){
    _wrist.setPeriodHertz(50);
    _wrist.attach(pin, 500, 2400);
};

void Wrist::disable(){
    _wrist.detach();
};

void Wrist::turnOn(){
    if(!servoOn){
        enable();
        servoOn = true;
    }
};

void Wrist::turnOff(){
    if(servoOn){
        disable();
        servoOn = false;
    }
}

void Wrist::setPos(float angleDeg){
    targetPos = _convertAngles(angleDeg);
    lastUpdateTime = millis();
    stepAccumulator = 0.0f;
}

void Wrist::setDuration(int input_speed){
    int constrained = constrain(input_speed, 1, 1000);
    speed = (float)constrained/1000;
    Serial.printf("Speed set: %f\n",speed);
}

void Wrist::move(int value){
    _wrist.writeMicroseconds(value);
}

void Wrist::step(){
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

bool Wrist::posCheck(){
    return currentPos == targetPos;
}

int Wrist::_convertAngles(float angleDeg){
    return map(constrain(angleDeg, wrists_commands_pos[0], wrists_commands_pos[1]), wrists_commands_pos[0], wrists_commands_pos[1], lowerLimit, upperLimit);
}

//### Functions ########################################################
void init_wrist_motors_setup_v2(){
    for(int i=0; i<WRIST_NUM; i++){
        wrists[i].init(wrists_pins[i], wrists_lowerLimits_pos[i],wrists_upperLimits_pos[i],wrists_default_pos[i],(float)1);
    }
}