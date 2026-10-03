#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include "LittleFS.h"

//### Variables and Constants ##########################################
extern const char* ssid;
extern const char* password;
extern bool iswas_wifi_connected;

extern WebServer server;



//### Functions ########################################################

void init_wifi_sever_setup();
void handleRoot();
void handleSetMulty();
void handleSetSingle();
void handlePose();