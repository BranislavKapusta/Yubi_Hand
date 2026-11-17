#pragma once
#include <Arduino.h>
#include "main_control.h"
#include <vector>
#include <algorithm>
#include "Serial/serial_com.h"

// Notes:

//### Structure
struct Dir_cmd_msg{
    int motor_id;
    float value;
};


//### Variables and Constants ##########################################
extern SemaphoreHandle_t finger_wrist_Mutex;
extern QueueHandle_t dirControl_cmd_Queue;

//### Functions ########################################################
void init_finger_wrist_mutex();
void motor_control_thread(void *pv);
void control_movement();
void direct_control_movement();