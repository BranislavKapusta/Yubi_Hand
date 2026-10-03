#include <Arduino.h>
#include "led_indicators.h"

// Notes:


//### Variables and Constants ##########################################
static const int Green_LED_primary_pin = 5;
static const int Green_LED_secondary_pin = 18;
static const int Yello_LED_pin = 19;

LED_indicator led_indicators[LED_INDICATORS_NUM];

//### Class LED indicator Functions ########################################################
void LED_indicator::init(int _pin, String _color){
    pin = _pin;
    color = _color;

    state = false;
    state_blink = false;
    state_on = false;
    time_length_on = 0;
    time_length_off = 0;
    startTime = 0;

    pinMode(pin, OUTPUT);
}

void LED_indicator::init_test(){
    on();
    delay(200);
    off();
}

void LED_indicator::on(){
    digitalWrite(pin, HIGH);
    state = true;
    startTime = millis();
}

void LED_indicator::off(){
    digitalWrite(pin, LOW);
    state = false;
    startTime = millis();
}

void LED_indicator::set_time(int time_on, int time_off){
    time_length_on = time_on;
    if(time_off > 0) time_length_off = time_off;
}

void LED_indicator::turn_on(int time){
    state_on = true;
    state_blink = false;
    set_time(time,0);
}

void LED_indicator::turn_off(){
    state_on = false;
    state_blink = false;
}

void LED_indicator::blink_on(int time_on, int time_off){
    state_on = false;
    state_blink = true;
    set_time(time_on,time_off);
}

void LED_indicator::update(){
    long elapsedTime = 0;
    
    if(state_on){
        if(time_length_on > 0){
            elapsedTime = millis() - startTime;
            if(state && elapsedTime > time_length_on){
                off();
                state_on = false;
            } else if(!state){
                on();
            }
        } else if(!state){
            on();
        }
    }
    if(state_blink){
        elapsedTime = millis() - startTime;
        if(state && elapsedTime > time_length_on){
            off();
        }else if(!state && elapsedTime > time_length_off){
            on();
        }
    }
}

//### Class Functions ########################################################
void init_led_indicators(){
    led_indicators[0].init(Green_LED_primary_pin, "Green P");
    led_indicators[1].init(Green_LED_secondary_pin, "Green S");
    led_indicators[2].init(Yello_LED_pin, "Yellow");

    //Test
    led_indicators[0].init_test();
    led_indicators[1].init_test();
    led_indicators[2].init_test();
}

void led_indicators_update(){
    for(int i=0; i < LED_INDICATORS_NUM; i++) led_indicators[i].update();
};