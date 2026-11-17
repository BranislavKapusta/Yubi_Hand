#pragma once
#include <Wire.h>
#include "FastIMU.h"

//### Class ##########################################
struct OrientationAnglesAccel{
    float pitchAngle;
    float rollAngle;
    float yawAngle;
};


//### Variables and Constants ##########################################


//### Functions ########################################################
void init_orientation_sensor();
void orientation_sensor_thread(void *pv);
void orientation_sensor_update();
void calibration_orientation_sensor();
OrientationAnglesAccel orientation_sensor_getAngles();
void orientation_sensor_read();
void calculate_accel_angles();
void calculate_integral_gyro();
