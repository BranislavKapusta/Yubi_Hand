#include "finger.h"
#include <ESP32Servo.h>


// Notes:


//### Variables and Constants ##########################################
const int fingers_pins[FINGER_NUM] = {13,14,27,25,26,33};
Servo finger[FINGER_NUM];

const int fingers_default_pos[FINGER_NUM] = {0,0,0,0,0,0};
const int fingers_upperLimits_pos[FINGER_NUM] = {2000,2000,2000,2000,2000,2000};
const int fingers_lowerLimits_pos[FINGER_NUM] = {1100,1100,1100,1100,1100,1200};

const int fingers_commands_pos[2] = {0,90};

int fingers_current_pos[FINGER_NUM];


//### Functions ########################################################
void init_finger_motors_setup(){
    for(int i=0; i<FINGER_NUM; i++){
        finger[i].setPeriodHertz(50);
        finger[i].attach(fingers_pins[i], 500, 2400);
    }
}

void finger_move(uint8_t index, float pos){
    if(index >= FINGER_NUM){
        Serial.println("Error - finger_move - wrong index");
        return;
    }
    
    int move_pos = finger_pos_convert(index, pos);

    finger[index].writeMicroseconds(move_pos);
    //Write down the new pos
    fingers_current_pos[index] = move_pos;
}

int finger_pos_convert(uint8_t index, float value){
    return map(constrain(value, fingers_commands_pos[0], fingers_commands_pos[1]), fingers_commands_pos[0], fingers_commands_pos[1], fingers_upperLimits_pos[index], fingers_lowerLimits_pos[index]);
}
