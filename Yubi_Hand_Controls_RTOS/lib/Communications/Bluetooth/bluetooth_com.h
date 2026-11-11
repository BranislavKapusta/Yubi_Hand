#pragma once
#include "BluetoothSerial.h"
#include "main_control.h"
#include "led_indicators.h"

//### Variables and Constants ##########################################
extern bool iswas_bluetooth_connected;

//### Functions ########################################################
void init_bluetooth_com();
void check_bluetooth_communication();
bool check_connected_bluetooth();
void process_bluetooth_message(String msg);