#include "bluetooth_com.h"
#include "BluetoothSerial.h"
#include "communications.h"

// Notes:


//### Variables and Constants ##########################################
BluetoothSerial serialBlueToothCom;
bool iswas_bluetooth_connected = false;

//### Functions ########################################################
void init_bluetooth_com(){
    serialBlueToothCom.begin("ProstheticHand");
    
    Serial.println("Bluetooth init - Done");
    //led_indicators[1].blink_on(500,1100); ----------------------------------------------------------------------------- <--- LEDs
}

void check_bluetooth_communication(){
    if(serialBlueToothCom.available()){
        //process_bluetooth_message(serialBlueToothCom.readStringUntil('\n'));
        process_communication_msgs("Bluetooth",serialBlueToothCom.readStringUntil('\n'));
    }

    if(check_connected_bluetooth()){
        if(!iswas_bluetooth_connected)iswas_bluetooth_connected = true;
        //led_indicators[1].turn_on(0); ----------------------------------------------------------------------------- <--- LEDs
    }else{
        //led_indicators[1].blink_on(500,1100); ----------------------------------------------------------------------------- <--- LEDs
        if(iswas_bluetooth_connected){
            iswas_bluetooth_connected = false;
            //safty_lossOfConnection(); ----------------------------------------------------------------------------- <--- Controls
        }
    }
};

bool check_connected_bluetooth(){
    return serialBlueToothCom.hasClient();
}
