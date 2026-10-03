#pragma once
#include <ESP32Servo.h>

#define WRIST_NUM 2

//### Variables and Constants ##########################################
extern const int wrist_pins[WRIST_NUM];
extern Servo wrist[WRIST_NUM];

extern const int wrist_default_pos[WRIST_NUM];
extern const int wrist_upperLimits_pos[WRIST_NUM];
extern const int wrist_lowerLimits_pos[WRIST_NUM];

extern int wrist_current_pos[WRIST_NUM];

extern int step;

//### Functions ########################################################
void init_wrist_motors_setup();
void wrist_move(uint8_t index, float pos);
int wrist_pos_convert(uint8_t index, float value);