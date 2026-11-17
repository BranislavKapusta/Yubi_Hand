#include <Arduino.h>
#include "main_control.h"
#include <vector>
#include <algorithm>
#include "Serial/serial_com.h"
#include "finger_wrist_control.h"

#include "finger.h"
#include "wrist.h"

// Notes:


//### Variables and Constants ##########################################
SemaphoreHandle_t finger_wrist_Mutex = NULL;

QueueHandle_t dirControl_cmd_Queue = NULL;

//### Functions ########################################################
void init_finger_wrist_mutex() {                     // call from setup(), before starting tasks
    finger_wrist_Mutex = xSemaphoreCreateMutex();
    dirControl_cmd_Queue = xQueueCreate(8, sizeof(Dir_cmd_msg));

    Serial.println("[ Info ] Motor Control - Init done");
}

void motor_control_thread(void *pv) {
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        direct_control_movement();
        control_movement();
        vTaskDelayUntil(&last, pdMS_TO_TICKS(100));
    }
}

void control_movement(){
    if (xSemaphoreTake(finger_wrist_Mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
        //Check fingers new opositions
        for(int i=0; i < FINGER_NUM; i++){
            if(!fingers[i].posCheck())fingers[i].step();
        }
        //Check wrist new opositions
        for(int i=0; i < WRIST_NUM; i++){
            if(!wrists[i].posCheck())wrists[i].step();
        }
        xSemaphoreGive(finger_wrist_Mutex);
    }
}

void direct_control_movement(){
    Dir_cmd_msg dirControl_msg;
    if(xQueueReceive(dirControl_cmd_Queue, &dirControl_msg, pdMS_TO_TICKS(5)) == pdTRUE){
        //Set the values
        if (xSemaphoreTake(finger_wrist_Mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
            if(dirControl_msg.motor_id >= 0 && dirControl_msg.motor_id < 6){
                fingers[dirControl_msg.motor_id].setPos(dirControl_msg.value);
            }else if(dirControl_msg.motor_id >= 6 && dirControl_msg.motor_id < 8){
                wrists[dirControl_msg.motor_id - 6].setPos(dirControl_msg.value);
            }
            xSemaphoreGive(finger_wrist_Mutex);
        }
    }
    
}