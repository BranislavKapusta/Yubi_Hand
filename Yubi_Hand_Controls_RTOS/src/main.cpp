#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BluetoothSerial.h>
#include <Wire.h>

#include "LittleFS.h"

#include "communications.h"
#include "main_control.h"
#include "finger_wrist_control.h"
#include "finger.h"
#include "wrist.h"
#include "led_indicators.h"


void setup() {
    init_led_indicators();
    init_communications();
    init_finger_wrist_mutex();
    init_finger_motors_setup();
    //init_wrist_motors_setup();
    init_main_control();

    
    xTaskCreatePinnedToCore(motor_control_thread, "motor_control", 4096, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(control_thread, "main_control", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(communication_thread, "communications", 8192, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(led_indicators_thread,   "led_indicators",   2048, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelete(NULL); 
}