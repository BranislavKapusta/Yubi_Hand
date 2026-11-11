#include <Arduino.h>

#include "Bluetooth/bluetooth_com.h"
#include "WiFi_server/wifi_server.h"
#include "Serial/serial_com.h"
#include "main_control.h"

void init_communications(){
    init_serial_com();
    init_bluetooth_com();
    init_wifi_sever_setup();
    Serial.println("[ Info ] COM - init done");
}

void communication_thread(void *pv) {
    for (;;) {
        server.handleClient();
        check_serial_communication();
        check_bluetooth_communication();
        vTaskDelay(pdMS_TO_TICKS(5));   // yield to other tasks
    }
}

void process_communication_msgs(String com_id, String msg){
    String infoMsg = "[ Info ] "+com_id+" - Recived: " + msg;
    String erroMsg = "";

    Serial.println(infoMsg);

    msg.trim();

    int marking_index = msg.indexOf(',');
    String marking = msg.substring(0, marking_index);

    if(marking.length() != 6){
        erroMsg = "[ Erro ] "+com_id+" - COM - Invalid marking length";
        Serial.println(erroMsg);
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

        infoMsg = "[ Info ] "+com_id+" COM - Recived: " + marking + String(n1) + String(n2) + String(n3) + String(n4) + String(n5) + String(n6) + String(n7) + String(n8);
        Serial.println(infoMsg);
        
        Pos_command serialCommunicationCommand = {(int)time, (int)n1, (int)n2, (int)n3, (int)n4, (int)n5, (int)n6, (int)n7, (int)n8};

        pos_command_add(serialCommunicationCommand);
        //Add command to the position commands
    }

    // Pose - pre defined poses of fingers
    if(marking == "GRIP01"){
        int pose_index = msg.indexOf(',', marking_index + 1);
        String pose = msg.substring(marking_index + 1, pose_index);

        infoMsg = "[ Info ] "+com_id+" COM - Recived: " + marking + pose;
        Serial.println(infoMsg);

        Act_command act_cmd = {0, "Pose", 0, pose};
        action_commands_add(act_cmd);
    }

    // Change speeds of the servos
    if(marking == "SPEED1"){

    }

    // Turn off wrist servos
    if(marking == "OFF001"){
        int cmd_index = msg.indexOf(',', marking_index + 1);
        int motor_num_index = msg.indexOf(',', cmd_index + 1);
        
        String cmd = msg.substring(marking_index + 1, cmd_index);
        int motor_num = msg.substring(cmd_index + 1, motor_num_index).toInt();;

        infoMsg = "[ Info ] "+com_id+" COM - Recived: " + marking + cmd + String(motor_num);
        Serial.println(infoMsg);

        Act_command act_cmd = {0, cmd, motor_num, "None"};
        action_commands_add(act_cmd);
    }
}