#include <vector>
#include <algorithm>

#include "control.h"

#include "wrist_v2.h"
#include "finger_v2.h"
#include "led_indicators.h"

#include "orientation_sensor.h"

// Notes:


//### Variables and Constants ##########################################
bool init_function_done = false;
std::vector<Pos_command> pos_command_buffer;
unsigned long lastCommandUpdateTime = 0;
unsigned long elapsedTime = 0;

//### Functions ########################################################
void init_fingers_wrist(){
    default_pose();
    init_function_done = true;
    Serial.println("F-W Init done");
}

void default_pose(){
    for (int i = 0; i < FINGER_NUM; i++) {
        fingers[i].setPos(fingers[i].defaultPos);  
    }
    for (int i = 0; i < WRIST_NUM; i++) {
        wrists[i].setPos(wrists[i].defaultPos);  
    }

}

void safty_lossOfConnection(){
    pos_command_buffer.push_back({0, 0, 0, 0, 0, 0, 0, -1, -1});
}

void control_movement(){
    
    //Check and set new pos by commands
    commands();

    auto_leveling_wrist();

    //Check fingers new opositions
    for(int i=0; i < FINGER_NUM; i++){
        if(!fingers[i].posCheck())fingers[i].step();
    }
    //Check wrist new opositions
    for(int i=0; i < WRIST_NUM; i++){
        if(!wrists[i].posCheck())wrists[i].step();
    }
}

void commands(){
    //Check new command and elapsed time
    if(pos_command_buffer.size() > 0){
        elapsedTime = millis() - lastCommandUpdateTime;

        //Update positions
        //Fingers
        if(pos_command_buffer.front().f_index_pos == -1){}
        else if(fingers[0].targetPos !=  pos_command_buffer.front().f_index_pos) fingers[0].setPos(pos_command_buffer.front().f_index_pos);

        if(pos_command_buffer.front().f_middle_pos == -1){}
        else if(fingers[1].targetPos !=  pos_command_buffer.front().f_middle_pos) fingers[1].setPos(pos_command_buffer.front().f_middle_pos);
        
        if(pos_command_buffer.front().f_ring_pos == -1){}
        else if(fingers[2].targetPos !=  pos_command_buffer.front().f_ring_pos) fingers[2].setPos(pos_command_buffer.front().f_ring_pos);
        
        if(pos_command_buffer.front().f_pinky_pos == -1){}
        else if(fingers[3].targetPos !=  pos_command_buffer.front().f_pinky_pos) fingers[3].setPos(pos_command_buffer.front().f_pinky_pos);
        
        if(pos_command_buffer.front().f_thumb_pos == -1){}
        else if(fingers[4].targetPos !=  pos_command_buffer.front().f_thumb_pos) fingers[4].setPos(pos_command_buffer.front().f_thumb_pos);
        
        if(pos_command_buffer.front().f_swing_pos == -1){}
        else if(fingers[5].targetPos !=  pos_command_buffer.front().f_swing_pos) fingers[5].setPos(pos_command_buffer.front().f_swing_pos);
        
        //Wrists
        if(pos_command_buffer.front().w_1_pos == -1){}
        else if(wrists[0].targetPos !=  pos_command_buffer.front().w_1_pos) wrists[0].setPos(pos_command_buffer.front().w_1_pos);

        if(pos_command_buffer.front().w_2_pos == -1){}
        else if(wrists[1].targetPos !=  pos_command_buffer.front().w_2_pos) wrists[1].setPos(pos_command_buffer.front().w_2_pos);

        if(elapsedTime > pos_command_buffer.front().time){
            //Delete the elapsed entry
            pos_command_buffer.erase(pos_command_buffer.begin());
            lastCommandUpdateTime = millis();
        }
    }else 
        lastCommandUpdateTime = millis();
}

void command_add(Pos_command input_command){
    pos_command_buffer.push_back(input_command);
}

void command_reset(){
    pos_command_buffer.clear();
}

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

void move_sequence(const char* pose){
    command_reset(); //Every new sequence is reseting for the previews commands

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

void move_animation(){}

void process_communication_msgs(String com_id,String msg){
    Serial.print(com_id);
    Serial.print(" Recived: ");
    Serial.println(msg);
    led_indicators[2].turn_on(500);

    msg.trim();

    int marking_index = msg.indexOf(',');
    String marking = msg.substring(0, marking_index);

    if(marking.length() != 6){
        Serial.print(com_id);
        Serial.println(" COM - Invalid marking length");
        return;
    }

    // Finger and Wrist pose commands, individual positions
    if(marking == "POS001"){
        //Msg format - XXXXXX, time, n1, n2, n3, n4, n5, n6, n7, n8

        //Index search
        int time_index = msg.indexOf(',', marking_index + 1);
        int num1_index = msg.indexOf(',', time_index + 1);
        int num2_index = msg.indexOf(',', num1_index + 1);
        int num3_index = msg.indexOf(',', num2_index + 1);
        int num4_index = msg.indexOf(',', num3_index + 1);
        int num5_index = msg.indexOf(',', num4_index + 1);
        int num6_index = msg.indexOf(',', num5_index + 1);
        int num7_index = msg.indexOf(',', num6_index + 1);
        int num8_index = msg.indexOf(',', num7_index + 1);

        //Spliting
        float time = msg.substring(marking_index + 1, time_index).toFloat();
        float n1 = msg.substring(time_index + 1, num1_index).toFloat();
        float n2 = msg.substring(num1_index + 1, num2_index).toFloat();
        float n3 = msg.substring(num2_index + 1, num3_index).toFloat();
        float n4 = msg.substring(num3_index + 1, num4_index).toFloat();
        float n5 = msg.substring(num4_index + 1, num5_index).toFloat();
        float n6 = msg.substring(num5_index + 1, num6_index).toFloat();
        float n7 = msg.substring(num6_index + 1, num7_index).toFloat();
        float n8 = msg.substring(num7_index + 1, num8_index).toFloat();

        Serial.print(com_id);
        Serial.println(" COM - Recived: " + marking + String(n1) + String(n2) + String(n3) + String(n4) + String(n5) + String(n6) + String(n7) + String(n8));
        Pos_command serialCommunicationCommand = {(int)time, (int)n1, (int)n2, (int)n3, (int)n4, (int)n5, (int)n6, (int)n7, (int)n8};
        command_add(serialCommunicationCommand);
    }

    // Pose - pre defined poses of fingers
    if(marking == "GRIP01"){
        int pose_index = msg.indexOf(',', marking_index + 1);
        String pose = msg.substring(marking_index + 1, pose_index);

        Serial.print(com_id);
        Serial.println(" COM - Recived: " + marking + pose);
        move_sequence(pose.c_str());
    }

    // Change speeds of the servos
    if(marking == "SPEED1"){

    }

    // Turn off wrist servos
    if(marking == "OFF001"){
        int cmd_index = msg.indexOf(',', marking_index + 1);
        int motor_num_index = msg.indexOf(',', cmd_index + 1);
        
        String cmd = msg.substring(marking_index + 1, cmd_index);
        int motor_index = msg.substring(cmd_index + 1, motor_num_index).toInt();;

        Serial.print(com_id);
        Serial.println(" COM - Recived: " + marking + cmd + String(motor_index));

        turnOnOff_motors(cmd,motor_index);
    }
}

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

void auto_leveling_wrist(){
    static unsigned long updateTime = 0;
    if(updateTime == 0) updateTime = millis();

    static Orientation_angles_accel angles;
    angles = orientation_sensor_getAngles();

    int cmdAngle = std::max(55, int(180 - abs(angles.rollAngle)));

    if( (millis() - updateTime) > 150 ){
        //Serial.print("---> Angle: ");
        //Serial.print(angles.rollAngle);
        //Serial.println(" <---");

        if( abs(angles.rollAngle) > 90 ){

            pos_command_buffer.push_back({0, -1, -1, -1, -1, -1, -1, cmdAngle, -1});
        }else{
            pos_command_buffer.push_back({0, -1, -1, -1, -1, -1, -1, 90, -1});
        }
        updateTime = millis();
    }
        
}