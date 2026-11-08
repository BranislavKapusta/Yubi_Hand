#include <Arduino.h>
#include "serial_com.h"
#include "communications.h"

void init_serial_com(){
    Serial.begin(115200);
}

void check_serial_communication(){
    if(Serial.available()){
        //process_serial_message(Serial.readStringUntil('\n'));
        process_communication_msgs("Serial",Serial.readStringUntil('\n'));
    }
}

