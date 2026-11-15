#include <Arduino.h>
#include "serial_com.h"
#include "communications.h"
#include "led_indicators.h"

static Led_cmd_msg cmd = {"",0, 1, 100, 100};

void init_serial_com(){
    Serial.begin(115200);
    Serial.println("[ Info ] Serial - init done");
}

void check_serial_communication(){
    if(Serial.available()){
        cmd = {"Serial_Recived",2, 0, 300, 0};
        xQueueSend(led_cmd_Queue, &cmd, 0);
        process_communication_msgs("Serial",Serial.readStringUntil('\n'));
    }
}

