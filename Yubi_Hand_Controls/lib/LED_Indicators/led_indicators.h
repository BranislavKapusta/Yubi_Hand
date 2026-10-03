#pragma once
#include <Arduino.h>

#define LED_INDICATORS_NUM 3

// Notes:


//### Class ##########################################
class LED_indicator {
public:
    int pin;
    String color;

    void init(int pin_num, String color);
    void turn_on(int time);
    void turn_off();
    void blink_on(int time_on, int time_off);
    void set_time(int time_on, int time_off);
    void update();

    void init_test();
private:
    void on();
    void off();
    bool state;
    bool state_blink;
    bool state_on;
    int time_length_on;
    int time_length_off;
    long startTime;
};

//### Variables and Constants ##########################################
extern LED_indicator led_indicators[LED_INDICATORS_NUM];

//### Functions ########################################################
void init_led_indicators();
void led_indicators_update();