#pragma once
#include "BluetoothSerial.h"

//### Variables and Constants ##########################################
extern bool iswas_bluetooth_connected;

//### Functions ########################################################
void init_bluetooth_com();
void check_bluetooth_communication();
bool check_connected_bluetooth();
void process_bluetooth_message(String msg);