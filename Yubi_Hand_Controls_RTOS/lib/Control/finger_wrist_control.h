#pragma once
#include <Arduino.h>
#include "main_control.h"
#include <vector>
#include <algorithm>
#include "Serial/serial_com.h"

// Notes:

//### Structure



//### Variables and Constants ##########################################
extern SemaphoreHandle_t finger_wrist_Mutex;

//### Functions ########################################################
void init_finger_wrist_mutex();
void motor_control_thread(void *pv);
void control_movement();