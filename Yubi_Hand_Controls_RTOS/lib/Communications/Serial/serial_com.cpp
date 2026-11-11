#include <Arduino.h>
#include "serial_com.h"
#include "communications.h"
#include "led_indicators.h"

static Led_cmd_msg cmd = {0, 1, 100, 100};

void init_serial_com(){
    Serial.begin(115200);
    Serial.println("[ Info ] Serial - init done");
}

void check_serial_communication(){
    if(Serial.available()){
        //process_serial_message(Serial.readStringUntil('\n'));
        process_communication_msgs("Serial",Serial.readStringUntil('\n'));
        
        //cmd = {1, 1, 500, 1100};
        //xQueueSend(led_cmd_Queue, &cmd, 0);
    }
}

