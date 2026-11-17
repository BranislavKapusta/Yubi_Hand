#include "orientation_sensor.h"
#include <Wire.h>
#include "FastIMU.h"

#include "finger_wrist_control.h"

//### Variables and Constants ##########################################
MPU6500 IMU;
calData calib = { 0 };

unsigned long prevTime;

bool init_orientation_calibration_bool = false;

AccelData accelData;
OrientationAnglesAccel accelAngles; 
OrientationAnglesAccel accelOffset;

GyroData gyroData;
float gyroX, gyroY, gyroZ;
float gyroAngleRoll, gyroAnglePitch, gyroAngleYaw;
float gyroAngleRollOffset, gyroAnglePitchOffset, gyroAngleYawOffset;

static String infMsg = "";

static Dir_cmd_msg dirControlQueueMsg = { -1, -1};

//### Class functions ##################################################

//### Functions ########################################################
void init_orientation_sensor(){
    int err = IMU.init(calib, 0x68);
    if (err != 0) {
        Serial.print("[ Erro ] Sen.Ori - Sensor init - Failed - ERROR: ");
        Serial.println(err);
    }else{
        Serial.println("[ Info ] Sen.Ori - Sensor init Done");
    }

    prevTime = millis();
}

void orientation_sensor_thread(void *pv) {
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        orientation_sensor_update();
        vTaskDelayUntil(&last, pdMS_TO_TICKS(100));
    }
}

void orientation_sensor_update(){
    if(!init_orientation_calibration_bool) calibration_orientation_sensor();

    orientation_sensor_read();
    calculate_accel_angles();
    
    //Send via dirrect control
    OrientationAnglesAccel angles = orientation_sensor_getAngles();

    infMsg ="[ TEST ] Angles: P > " + String(angles.pitchAngle) + " deg" + " R >" + String(angles.rollAngle) + " deg" + " Y >" + String(angles.yawAngle) + " deg";
    Serial.println(infMsg);

    //Send via dirrect control
    if(angles.pitchAngle > 90){
        dirControlQueueMsg = {6, (float(180) - angles.pitchAngle)};
        xQueueSend(dirControl_cmd_Queue, &dirControlQueueMsg, 0); 
    }else{
        dirControlQueueMsg = {6, 90};
        xQueueSend(dirControl_cmd_Queue, &dirControlQueueMsg, 0); 
    }
}

OrientationAnglesAccel orientation_sensor_getAngles(){
    OrientationAnglesAccel angles;
    angles.pitchAngle = accelAngles.pitchAngle - accelOffset.pitchAngle;
    angles.rollAngle = accelAngles.rollAngle - accelOffset.rollAngle;
    angles.yawAngle = accelAngles.yawAngle - accelOffset.yawAngle;

    return angles;
}

void calibration_orientation_sensor(){
    static unsigned long startTime = 0;

    static int numberOfData = 0;
    static double accelPichAccumulated = 0;
    static double accelRollAccumulated = 0;
    static double accelYawAccumulated = 0;

    if(startTime == 0) startTime = millis();

    if( millis() - startTime < 1000){
        orientation_sensor_read();
        calculate_accel_angles();

        accelPichAccumulated += accelAngles.pitchAngle;
        accelRollAccumulated += accelAngles.rollAngle;
        accelYawAccumulated += accelAngles.yawAngle;
        numberOfData++;
    }else{
        accelOffset.pitchAngle = accelPichAccumulated/numberOfData;
        accelOffset.rollAngle = accelRollAccumulated/numberOfData;
        accelOffset.yawAngle = accelYawAccumulated/numberOfData;

        init_orientation_calibration_bool = true;
    }
}

void orientation_sensor_read(){
    IMU.update();
    IMU.getAccel(&accelData);
    IMU.getGyro(&gyroData);
}

void calculate_accel_angles(){
    accelAngles.rollAngle = atan(accelData.accelY / sqrt(accelData.accelX*accelData.accelX + accelData.accelZ*accelData.accelZ))*1/(M_PI/180);
    accelAngles.pitchAngle = -atan(accelData.accelX / sqrt(accelData.accelY*accelData.accelY + accelData.accelZ*accelData.accelZ))*1/(M_PI/180);
    accelAngles.yawAngle = atan(accelData.accelZ / sqrt(accelData.accelY*accelData.accelY + accelData.accelX*accelData.accelX))*1/(M_PI/180);
}

void calculateIntegralGyro(){
    unsigned long now = millis();
    unsigned long dt = now - prevTime;
    prevTime = now;

    gyroAnglePitch += gyroData.gyroX * dt/1000;
    gyroAngleRoll += gyroData.gyroY * dt/1000;
    gyroAngleYaw += gyroData.gyroZ * dt/1000;
    
}