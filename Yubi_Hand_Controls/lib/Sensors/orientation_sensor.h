#pragma once
#include <Wire.h>
#include "FastIMU.h"

//### Class ##########################################
struct Orientation_angles_accel{
    float pitchAngle;
    float rollAngle;
    float yawAngle;
};

//### Variables and Constants ##########################################


//### Functions ########################################################
void init_orientation_sensor();
void orientation_sensor_update();
void init_calibration_orientation_sensor();
Orientation_angles_accel orientation_sensor_getAngles();
void orientationSensorRead();
void calculateAcceAngles();
void calculateIntegralGyro();
