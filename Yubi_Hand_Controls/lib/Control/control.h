#pragma once
#include "finger.h"
#include "wrist.h"
#include "led_indicators.h"
#include <vector>

// Notes:

//### Structure
struct Pos_command{
    int time; //if 0 it is set immediately, no waiting
    int f_index_pos;
    int f_middle_pos;
    int f_ring_pos;
    int f_pinky_pos;
    int f_thumb_pos;
    int f_swing_pos;
    int w_1_pos;
    int w_2_pos;
};


//### Variables and Constants ##########################################
extern bool init_function_done;

extern std::vector<Pos_command> pos_command_buffer;
extern unsigned long lastCommandUpdateTime;
extern unsigned long elapsedTime;

//### Functions ########################################################
void init_fingers_wrist();
void control_movement();
void commands();
void command_add(Pos_command input_command);
void command_reset();
void default_pose();
void safty_lossOfConnection();
void move_single(uint8_t index, float pos);
void move_multy(float pos[8]);
void move_sequence(const char* pose);
void move_animation();
void process_communication_msgs(String com_id,String msg);
void turnOnOff_motors(String cmd_type, int motor_index);
void auto_leveling_wrist();