#pragma once
#include <Arduino.h>
#include "main_control.h"
#include <vector>
#include <algorithm>
#include "Serial/serial_com.h"

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
extern std::vector<Pos_command> action_command_buffer;
extern SemaphoreHandle_t commands_Mutex;
extern unsigned long lastCommandUpdateTime;
extern unsigned long elapsedTime;

//### Functions ########################################################
void init_main_control();
void control_thread(void *pv);

void pos_commands();
bool pos_command_add(Pos_command input_command);
void pos_command_reset();

void action_commands();
void action_commands_add();
void action_command_reset();

void default_pose();
void safty_lossOfConnection();
//void move_single(uint8_t index, float pos);
//void move_multy(float pos[8]);
void move_sequence(const char* pose);

void turnOnOff_motors(String cmd_type, int motor_index);