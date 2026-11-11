#pragma once
#include <Arduino.h>
#include "communications.h"
#include "led_indicators.h"

//### Class ##########################################


//### Variables and Constants ##########################################


//### Functions ########################################################
void init_serial_com();
void check_serial_communication();
void process_serial_message(String msg);
