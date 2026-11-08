#pragma once
#include <Arduino.h>
#include "Bluetooth/bluetooth_com.h"
#include "WiFi_server/wifi_server.h"
#include "Serial/serial_com.h"

// Notes:

//### Structure


//### Variables and Constants ##########################################


//### Functions ########################################################
void init_communications();
void communication_thread(void *pv);
void process_communication_msgs(String com_id, String msg);