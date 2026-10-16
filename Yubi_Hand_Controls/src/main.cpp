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

#include "orientation_sensor.h"
#include "FastIMU.h"

MPU6500 IMU1;
calData calib1 = { 0 };


void setup() {
    Wire.begin();
    delay(1000); // needed for the wire initialization - dont know why

    init_serial_com();
    init_bluetooth_com();

    init_led_indicators();
    init_finger_motors_setup_v2();
    init_wrist_motors_setup_v2();

    init_wifi_sever_setup();

    init_fingers_wrist();

    init_orientation_sensor();

}

void loop() {
    // Communications
    server.handleClient();
    check_serial_communication();
    check_bluetooth_communication();
    
    // Sensors
    orientation_sensor_update();

    // Motor control
    control_movement();

    led_indicators_update();
}