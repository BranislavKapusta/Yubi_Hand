#include "wifi_server.h"
#include <WiFi.h>
#include <WebServer.h>
#include "LittleFS.h"
#include "control.h"
#include "led_indicators.h"

// Notes:
// Wifi - IP address: 192.168.4.1


//### Variables and Constants ##########################################
const char* ssid = "ESP32-Hand";
const char* password = "handcontrol";

bool iswas_wifi_connected = false;

WebServer server(80);


//### Functions ########################################################
void init_wifi_sever_setup(){
    // Start filesystem
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    //Wifi setup
    if (!WiFi.softAP(ssid, password)) {
        Serial.println("WiFi AP Failed to start");
        led_indicators[2].blink_on(500,500);
        return;
    }

    led_indicators[0].blink_on(500,1100);
    
    server.on("/", handleRoot);
    server.on("/setsingle", handleSetSingle);
    server.on("/setmulty", handleSetMulty);
    server.on("/pose", handlePose);
    
    server.on("/script.js", []() {
        File file = LittleFS.open("/script.js", "r");
        if (!file) { server.send(404, "text/plain", "Not found"); return; }
        server.streamFile(file, "application/javascript");
        file.close();
    });

    server.onNotFound([]() {
        Serial.print("NOT FOUND: ");
        Serial.println(server.uri());
        server.send(404, "text/plain", "Not found");
    });

    // Connection check
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("Client connected");
        led_indicators[0].turn_on(0);
        if(!iswas_wifi_connected)iswas_wifi_connected = true;
    }, ARDUINO_EVENT_WIFI_AP_STACONNECTED);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("Client disconnected");
        led_indicators[0].blink_on(500,1100);
        if(iswas_wifi_connected){
            iswas_wifi_connected = false;
            safty_lossOfConnection();
        }
    }, ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

    server.begin();
}

void handleSetSingle() {
    Serial.println("Received CMD: Set Single");
    led_indicators[2].turn_on(500);

    if (server.hasArg("index") && server.hasArg("val")) {
        int index = server.arg("index").toInt();
        int val = server.arg("val").toInt();
        
        move_single(index,val);
    }
    server.send(200, "text/plain", "OK");
}

void handleSetMulty() {
    float values[8];
    bool allFound = true;
    led_indicators[2].turn_on(500);

    for (int i = 0; i < 8; i++) {
        String key = "v" + String(i);
        if (server.hasArg(key)) {
            values[i] = server.arg(key).toInt();
            Serial.print(key + ": ");
            Serial.println(values[i]);
        } else {
            Serial.print("Missing: ");
            Serial.println(key);
            allFound = false;
        }
    }

    if (allFound) {
        // Use your values here
        move_multy(values);
    }

    server.send(200, "text/plain", "OK");
}

void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

void handlePose() {
    led_indicators[2].turn_on(500);

    if (server.hasArg("name")) {
        String name = server.arg("name");
        Serial.print("Pose: "); Serial.println(name);

        if (name == "open") move_sequence(name.c_str());
        if (name == "fist") move_sequence(name.c_str());
        if (name == "point") move_sequence(name.c_str());
        if (name == "pinch") move_sequence(name.c_str());
        if (name == "3pinch") move_sequence(name.c_str());
        if (name == "cilinder") move_sequence(name.c_str());
        if (name == "small_cilinder") move_sequence(name.c_str());
        if (name == "relax") move_sequence(name.c_str());
    }
    server.send(200, "text/plain", "OK");
}