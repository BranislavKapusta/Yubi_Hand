#include "wrist.h"
#include <ESP32Servo.h>


// Notes:


//### Variables and Constants ##########################################
const int wrist_pins[WRIST_NUM] = {22,23};
Servo wrist[WRIST_NUM];

const int wrist_default_pos[WRIST_NUM] = {90,90};
const int wrist_upperLimits_pos[WRIST_NUM] = {135,135};
const int wrist_lowerLimits_pos[WRIST_NUM] = {45,45};

int wrist_current_pos[WRIST_NUM] = {89,89};

int step = 1;

//### Functions ########################################################
void init_wrist_motors_setup(){
    for(int i=0; i<WRIST_NUM; i++){
        wrist[i].setPeriodHertz(50);
        wrist[i].attach(wrist_pins[i], 500, 2400);
    }
}

void wrist_move(uint8_t index, float pos){
    if(index >= WRIST_NUM){
        Serial.println("Error - wrist_move - wrong index");
        return;
    }
    
    int move_pos = wrist_pos_convert(index, pos);

    while(wrist_current_pos[index] != move_pos){
        if(wrist_current_pos[index] < move_pos) wrist_current_pos[index] = wrist_current_pos[index] + step;
        if(wrist_current_pos[index] > move_pos) wrist_current_pos[index] = wrist_current_pos[index] - step;
        delay(25);
        wrist[index].write(wrist_current_pos[index]);
    }

    //Write down the pos
    wrist_current_pos[index] = move_pos;
}

int wrist_pos_convert(uint8_t index, float value){
    return map(constrain(value, 20, 160), 20, 160, wrist_lowerLimits_pos[index], wrist_upperLimits_pos[index]);
}
