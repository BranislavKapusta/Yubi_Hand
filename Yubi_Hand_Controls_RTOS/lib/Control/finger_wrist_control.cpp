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

//### Functions ########################################################
void init_finger_wrist_mutex() {                     // call from setup(), before starting tasks
    finger_wrist_Mutex = xSemaphoreCreateMutex();
}

void motor_control_thread(void *pv) {
    TickType_t last = xTaskGetTickCount();
    for (;;) {
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