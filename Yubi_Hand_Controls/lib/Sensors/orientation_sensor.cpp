#include "orientation_sensor.h"
#include <Wire.h>
#include "FastIMU.h"


//### Variables and Constants ##########################################
MPU6500 IMU;

calData calib = { 0 };

unsigned long prevTime;

bool init_orientation_calibration_bool = false;

AccelData accelData;
float accelX, accelY, accelZ;
float accelAngleRoll, accelAnglePitch, accelAngleYaw;
float accelAngleRollOffset, accelAnglePitchOffset, accelAngleYawOffset;

GyroData gyroData;
float gyroX, gyroY, gyroZ;
float gyroAngleRoll, gyroAnglePitch, gyroAngleYaw;
float gyroAngleRollOffset, gyroAnglePitchOffset, gyroAngleYawOffset;

//### Class functions ##################################################

//### Functions ########################################################
void init_orientation_sensor(){
    int err = IMU.init(calib, 0x68);
    if (err != 0) {
        Serial.print("Orientation Sensor init - Failed - ERROR: ");
        Serial.println(err);
    }else{
        Serial.println("Orientation Sensor init - Done");
    }

    prevTime = millis();

}

void orientation_sensor_update(){
    if(!init_orientation_calibration_bool)init_calibration_orientation_sensor();

    orientationSensorRead();
    calculateAcceAngles();
    
    /*
    Serial.print("Offset Angles - Pitch: ");
    Serial.print(accelAnglePitchOffset);
    Serial.print(" Roll: ");
    Serial.print(accelAngleRollOffset);
    Serial.print(" Yaw: ");
    Serial.print(accelAngleYawOffset);
    Serial.println(" - ");
    
    Serial.print("Angles - Pitch: ");
    Serial.print( (accelAnglePitch - accelAnglePitchOffset ));
    Serial.print(" Roll: ");
    Serial.print( (accelAngleRoll - accelAngleRollOffset));
    Serial.print(" Yaw: ");
    Serial.print( (accelAngleYaw - accelAngleYawOffset));
    Serial.println(" - ");
    */    
}

Orientation_angles_accel orientation_sensor_getAngles(){
    Orientation_angles_accel angles;
    angles.pitchAngle = accelAnglePitch - accelAnglePitchOffset;
    angles.rollAngle = accelAngleRoll - accelAngleRollOffset;
    angles.yawAngle = accelAngleYaw - accelAngleYawOffset;

    return angles;
}

void init_calibration_orientation_sensor(){
    static unsigned long startTime = 0;

    static int numberOfData = 0;
    static double accelPichAccumulated = 0;
    static double accelRollAccumulated = 0;
    static double accelYawAccumulated = 0;

    if(startTime == 0) startTime = millis();

    if( millis() - startTime < 1000){
        orientationSensorRead();
        calculateAcceAngles();

        accelPichAccumulated += accelAnglePitch;
        accelRollAccumulated += accelAngleRoll;
        accelYawAccumulated += accelAngleYaw;
        numberOfData++;
    }else{
        accelAnglePitchOffset = accelPichAccumulated/numberOfData;
        accelAngleRollOffset = accelRollAccumulated/numberOfData;
        accelAngleYawOffset = accelYawAccumulated/numberOfData;

        init_orientation_calibration_bool = true;
    }
}

void orientationSensorRead(){
    IMU.update();
    IMU.getAccel(&accelData);
    IMU.getGyro(&gyroData);
}

void calculateAcceAngles(){
    accelX = accelData.accelX;
    accelY = accelData.accelY;
    accelZ = accelData.accelZ;

    accelAngleRoll = atan(accelY / sqrt(accelX*accelX + accelZ*accelZ))*1/(3.142/180);
    accelAnglePitch = -atan(accelX / sqrt(accelY*accelY + accelZ*accelZ))*1/(3.142/180);
    accelAngleYaw = atan(accelZ / sqrt(accelY*accelY + accelX*accelX))*1/(3.142/180);
}

void calculateIntegralGyro(){
    unsigned long now = millis();
    unsigned long dt = now - prevTime;
    prevTime = now;

    gyroAnglePitch += gyroData.gyroX * dt/1000;
    gyroAngleRoll += gyroData.gyroY * dt/1000;
    gyroAngleYaw += gyroData.gyroZ * dt/1000;
    
}