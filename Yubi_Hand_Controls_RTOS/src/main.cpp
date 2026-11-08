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


void ledTask(void *pv) {
    for (;;) {
        //led_indicators_update();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void setup() {
    init_finger_wrist_mutex();
    init_main_control();
    init_communications();
    /*
    init_led_indicators();
    */
    init_finger_motors_setup();
    //init_wrist_motors_setup();
   
    xTaskCreatePinnedToCore(motor_control_thread, "motorControl", 4096, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(control_thread, "control", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(communication_thread, "comms", 8192, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(ledTask,   "leds",   2048, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelete(NULL); 
}