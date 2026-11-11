#include "bluetooth_com.h"
#include "BluetoothSerial.h"
#include "main_control.h"
#include "led_indicators.h"

// Notes:


//### Variables and Constants ##########################################
BluetoothSerial serialBlueToothCom;
bool iswas_bluetooth_connected = false;
static Led_cmd_msg cmd = {0, 1, 100, 100};

//### Functions ########################################################
void init_bluetooth_com(){
    serialBlueToothCom.begin("ProstheticHand");
    
    Serial.println("[ Info ] Bluetooth - init done");
    cmd = {1, 1, 500, 1100};
    xQueueSend(led_cmd_Queue, &cmd, 0);
}

void check_bluetooth_communication(){
    if(serialBlueToothCom.available()){
        //process_bluetooth_message(serialBlueToothCom.readStringUntil('\n'));
        process_communication_msgs("Bluetooth",serialBlueToothCom.readStringUntil('\n'));
    }

    if(check_connected_bluetooth()){
        if(!iswas_bluetooth_connected)iswas_bluetooth_connected = true;
        cmd = {1, 0, 0, 0};
        xQueueSend(led_cmd_Queue, &cmd, 0);
    }else{
        cmd = {1, 1, 500, 1100};
        xQueueSend(led_cmd_Queue, &cmd, 0);
        if(iswas_bluetooth_connected){
            iswas_bluetooth_connected = false;
            //safty_lossOfConnection(); ----------------------------------------------------------------------------- <--- Controls
        }
    }
};

bool check_connected_bluetooth(){
    return serialBlueToothCom.hasClient();
}
