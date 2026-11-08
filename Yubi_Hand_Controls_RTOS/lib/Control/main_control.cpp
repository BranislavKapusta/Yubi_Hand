#include <Arduino.h>
#include "main_control.h"
#include <vector>
#include <algorithm>
#include "Serial/serial_com.h"

#include "finger.h"
#include "wrist.h"
#include "finger_wrist_control.h"

// Notes:


//### Variables and Constants ##########################################
bool init_function_done = false;
std::vector<Pos_command> pos_command_buffer;
std::vector<Pos_command> action_command_buffer;
SemaphoreHandle_t commands_Mutex = NULL; 
unsigned long lastCommandUpdateTime = 0;
unsigned long elapsedTime = 0;

//### Functions ########################################################
void control_thread(void *pv) {
    for (;;) {
        pos_commands();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void init_main_control(){
    commands_Mutex = xSemaphoreCreateMutex();
    pos_command_buffer.reserve(64);
    action_command_buffer.reserve(64);

    default_pose();
    init_function_done = true;
    Serial.println("[ Info ] Control - F-W Init done");
}

void pos_commands(){
    Pos_command cmd;
    bool newTimeCMD = false;

    if(xSemaphoreTake(commands_Mutex, pdMS_TO_TICKS(10)) == pdTRUE){
        if(pos_command_buffer.size() > 0){
            cmd = pos_command_buffer.front();
            newTimeCMD = true;
        }
        xSemaphoreGive(commands_Mutex);
    }

    //Check new command and elapsed time
    if(newTimeCMD){
        elapsedTime = millis() - lastCommandUpdateTime;

        //Update positions
        if(xSemaphoreTake(finger_wrist_Mutex, pdMS_TO_TICKS(10)) == pdTRUE){
            //Fingers
            if(cmd.f_index_pos == -1){}
            else if(fingers[0].targetPos !=  cmd.f_index_pos) fingers[0].setPos(cmd.f_index_pos);

            if(cmd.f_middle_pos == -1){}
            else if(fingers[1].targetPos !=  cmd.f_middle_pos) fingers[1].setPos(cmd.f_middle_pos);
            
            if(cmd.f_ring_pos == -1){}
            else if(fingers[2].targetPos !=  cmd.f_ring_pos) fingers[2].setPos(cmd.f_ring_pos);
            
            if(cmd.f_pinky_pos == -1){}
            else if(fingers[3].targetPos !=  cmd.f_pinky_pos) fingers[3].setPos(cmd.f_pinky_pos);
            
            if(cmd.f_thumb_pos == -1){}
            else if(fingers[4].targetPos !=  cmd.f_thumb_pos) fingers[4].setPos(cmd.f_thumb_pos);
            
            if(cmd.f_swing_pos == -1){}
            else if(fingers[5].targetPos !=  cmd.f_swing_pos) fingers[5].setPos(cmd.f_swing_pos);
            
            //Wrists
            if(cmd.w_1_pos == -1){}
            else if(wrists[0].targetPos !=  cmd.w_1_pos) wrists[0].setPos(cmd.w_1_pos);

            if(cmd.w_2_pos == -1){}
            else if(wrists[1].targetPos !=  cmd.w_2_pos) wrists[1].setPos(cmd.w_2_pos);
            xSemaphoreGive(finger_wrist_Mutex);
        }
        
        //Delete the elapsed entry
        if(elapsedTime > cmd.time){
            //Delete the elapsed entry
            if(xSemaphoreTake(commands_Mutex, pdMS_TO_TICKS(10)) == pdTRUE){
                pos_command_buffer.erase(pos_command_buffer.begin());
                xSemaphoreGive(commands_Mutex);
                lastCommandUpdateTime = millis();
            }
        }
    }else 
        lastCommandUpdateTime = millis();
}

bool pos_command_add(Pos_command input_command){
    bool result = false;

    if (xSemaphoreTake(commands_Mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        pos_command_buffer.push_back(input_command);
        xSemaphoreGive(commands_Mutex);
        result = true;
    }

    return result;
}

void pos_command_reset(){
    if (xSemaphoreTake(commands_Mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        pos_command_buffer.clear();
        xSemaphoreGive(commands_Mutex);
    }
}

void action_commands(){

}

void action_commands_add(){

}

void action_command_reset(){

}

void default_pose(){
    if(xSemaphoreTake(finger_wrist_Mutex, pdMS_TO_TICKS(10)) == pdTRUE){
        for (int i = 0; i < FINGER_NUM; i++) {
            fingers[i].setPos(fingers[i].defaultPos);  
        }
        for (int i = 0; i < WRIST_NUM; i++) {
            wrists[i].setPos(wrists[i].defaultPos);  
        }
        xSemaphoreGive(finger_wrist_Mutex);
        Serial.println("[ Info ] Control - Default positons set");
    }else
        Serial.println("[ Erro ] Control - Mutex Default positons - failed");
}

void safty_lossOfConnection(){
    if(xSemaphoreTake(commands_Mutex, pdMS_TO_TICKS(10)) == pdTRUE){
        pos_command_buffer.push_back({0, 0, 0, 0, 0, 0, 0, -1, -1});
        xSemaphoreGive(commands_Mutex);
    }
}

/*
Rework to command style with 0 wait or leave it like this because there can be prewious commands - so make check to delete prewious commands
void move_single(uint8_t index, float pos){
    if(index < FINGER_NUM){
        fingers[index].setPos(pos);
    }else if(index > (FINGER_NUM-1) && index < (FINGER_NUM+WRIST_NUM)){
        wrists[index - FINGER_NUM].setPos(pos);
    }
}

void move_multy(float pos[8]){
    for(int i = 0; i < FINGER_NUM; i++){
        fingers[i].setPos(pos[i]);    
    }
    for(int i=0; i < WRIST_NUM; i++){
        wrists[i].setPos(pos[i+FINGER_NUM]);
    }
    
}
*/
/*
void move_sequence(const char* pose){
    pos_command_reset(); //Every new sequence is reseting for the previews commands

    //pos_command_buffer.push_back({1000, -1, -1, -1, -1, -1, -1, -1, -1});

    if(strcmp(pose, "open") == 0){
        pos_command_buffer.push_back({0, 0, 0, 0, 0, 0, 0, -1, -1});

    }else if(strcmp(pose, "fist") == 0){
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 0, 0, -1, -1});
        pos_command_buffer.push_back({1500, 85, 85, 85, 85, 0, 0, -1, -1});
        pos_command_buffer.push_back({0, -1, -1, -1, -1, 50, 85, -1, -1});

    }else if(strcmp(pose, "pinch") == 0){
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 0, 0, -1, -1});
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 40, 60, -1, -1});
        pos_command_buffer.push_back({0, 47, 0, 0, 0, -1, -1, -1, -1});

    }else if(strcmp(pose, "3pinch") == 0){
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 0, 0, -1, -1});
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 40, 65, -1, -1});
        pos_command_buffer.push_back({0, 47, 47, 0, 0, -1, -1, -1, -1});

    }else if(strcmp(pose, "point") == 0){
        pos_command_buffer.push_back({1000, 0, 85, 85, 85, 0, 0, -1, -1});
        pos_command_buffer.push_back({1000, 0, 85, 85, 85, 50, 85, -1, -1});

    }else if(strcmp(pose, "cilinder") == 0){
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 0, 0, -1, -1});//Reset

        pos_command_buffer.push_back({1000, 0, 0, 0, 0, 10, 90, -1, -1});
        pos_command_buffer.push_back({100, -1, -1, -1, 30, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, -1, 30, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, 30, -1, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({  0, 30, -1, -1, -1, -1, -1, -1, -1});

    }else if(strcmp(pose, "small_cilinder") == 0){
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 0, 0, -1, -1});//Reset

        pos_command_buffer.push_back({1000, 0, 0, 0, 0, 10, 90, -1, -1});
        pos_command_buffer.push_back({100, -1, -1, -1, 50, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, -1, 50, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, 50, -1, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({  0, 50, -1, -1, -1, -1, -1, -1, -1});

    }else if(strcmp(pose, "relax") == 0){
        pos_command_buffer.push_back({2000, 0, 0, 0, 0, 0, 0, -1, -1});//Reset

        pos_command_buffer.push_back({100, -1, -1, -1, 21, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, -1, 13, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, 12, -1, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, 10, -1, -1, -1, -1, -1, -1, -1});
        pos_command_buffer.push_back({100, -1, -1, -1, -1, 15, 23, -1, -1});
        
    }else{
        Serial.print("Error - pose not defined - pose: ");
        Serial.println(pose);
    }
    
}
*/
/*
void turnOnOff_motors(String cmd_type, int motor_index){
    if( (cmd_type == "on" || cmd_type == " on")  && motor_index < 8){
        if(motor_index >= 0 && motor_index < 6){
            fingers[motor_index].turnOn();
        }else if(motor_index >= 6 && motor_index < 8){
            wrists[motor_index - 6].turnOn();
        }
    }else if( (cmd_type == "off" ||  cmd_type == " off") && motor_index < 8){
        if(motor_index >= 0 && motor_index < 6){
            fingers[motor_index].turnOff();
        }else if(motor_index >= 6 && motor_index < 8){
            wrists[motor_index - 6].turnOff();
        }
    }else
        Serial.println("Funcion turn on/off ERROR");
}
*/