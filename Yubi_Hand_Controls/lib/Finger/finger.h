#pragma once
#include <ESP32Servo.h>

#define FINGER_NUM 6

//### Variables and Constants ##########################################
extern const int fingers_pins[FINGER_NUM];
extern Servo finger[FINGER_NUM];

extern const int fingers_default_pos[FINGER_NUM];
extern const int fingers_upperLimits_pos[FINGER_NUM];
extern const int fingers_lowerLimits_pos[FINGER_NUM];

extern const int fingers_commands_pos[2];

extern int fingers_current_pos[FINGER_NUM];

//### Functions ########################################################
void init_finger_motors_setup();
void finger_move(uint8_t index, float pos);
int finger_pos_convert(uint8_t index, float value);
