#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BluetoothSerial.h>
#include <Wire.h>

#include "LittleFS.h"

#include "wifi_server.h"
#include "serial_com.h"
#include "bluetooth_com.h"

#include "finger_v2.h"
#include "wrist_v2.h"
#include "led_indicators.h"
#include "control.h"


void setup() {
    init_serial_com();
    init_bluetooth_com();

    init_led_indicators();
    init_finger_motors_setup_v2();
    init_wrist_motors_setup_v2();

    init_wifi_sever_setup();

    init_fingers_wrist();

}

void loop() {
    // Communications
    server.handleClient();
    check_serial_communication();
    check_bluetooth_communication();

    // Motor control
    control_movement();

    led_indicators_update();
}