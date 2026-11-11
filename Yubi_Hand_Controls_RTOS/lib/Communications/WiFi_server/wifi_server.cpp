#include "wifi_server.h"
#include <WiFi.h>
#include <WebServer.h>
#include "LittleFS.h"
#include "communications.h"
#include "main_control.h"
#include "led_indicators.h"

// Notes:
// Wifi - IP address: 192.168.4.1


//### Variables and Constants ##########################################
const char* ssid = "ESP32-Hand";
const char* password = "handcontrol";

bool iswas_wifi_connected = false;

WebServer server(80);

static Led_cmd_msg cmd = {0, 1, 100, 100};

String errMsg = "";
String infMsg = "";

String msg = "";

//### Functions ########################################################
void init_wifi_sever_setup(){
    // Start filesystem
    if (!LittleFS.begin()) {
        errMsg = "[ Erro ] WiFi - LittleFS Mount Failed";
        Serial.println(errMsg);
        return;
    }

    //Wifi setup
    if (!WiFi.softAP(ssid, password)) {
        errMsg = "[ Erro ] WiFi - WiFi AP Failed to start";
        Serial.println(errMsg);
        cmd = {2, 1, 500, 500}; xQueueSend(led_cmd_Queue, &cmd, 0);
        return;
    }

    cmd = {0, 1, 500, 1100}; xQueueSend(led_cmd_Queue, &cmd, 0);

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
        errMsg = "[ Erro ] WiFi - not found: ";
        Serial.print(errMsg);
        Serial.println(server.uri());
        server.send(404, "text/plain", "Not found");
    });

    // Connection check
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        infMsg = "[ Info ] WiFi - Client connected";
        Serial.println(infMsg);
        cmd = {0, 0, 0, 0}; xQueueSend(led_cmd_Queue, &cmd, 0);

        if(!iswas_wifi_connected)iswas_wifi_connected = true;
    }, ARDUINO_EVENT_WIFI_AP_STACONNECTED);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        infMsg = "[ Aler ] WiFi - Client disconnected";
        Serial.println(infMsg);
        cmd = {0, 1, 500, 1100}; xQueueSend(led_cmd_Queue, &cmd, 0);
        if(iswas_wifi_connected){
            iswas_wifi_connected = false;
            for(int i = 0; i < 3; i++){
                if(safty_lossOfConnection() == true) break;
                vTaskDelay(pdMS_TO_TICKS(20));
            }
        }
    }, ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

    server.begin();
    Serial.println("[ Info ] WiFi - init done");
}

void handleSetSingle() {
    infMsg = "[ Info ] WiFi - Received CMD - Set Single";
    Serial.println(infMsg);
    cmd = {2, 0, 500, 0}; xQueueSend(led_cmd_Queue, &cmd, 0);

    if (server.hasArg("index") && server.hasArg("val")) {
        int index = server.arg("index").toInt();
        int val = server.arg("val").toInt();
        
        //move_single(index,val); ----------------------------------------------------------------------------- <--- Controls
        //Build the msg
        msg = "POS001, 0";
        for (int i = 0; i < 8; i++){
            if(i == index){
                msg +=",";
                msg += String(val);
            }else{
                msg +=",-1";
            }
        }
        process_communication_msgs("WiFi",msg);
    }
    server.send(200, "text/plain", "OK");
}

void handleSetMulty() {
    float values[8];
    bool allFound = true;
    cmd = {2, 0, 500, 0}; xQueueSend(led_cmd_Queue, &cmd, 0);

    infMsg = "[ Info ] WiFi - Recived multy set position";
    Serial.println(infMsg);

    for (int i = 0; i < 8; i++) {
        String key = "v" + String(i);
        if (server.hasArg(key)) {
            values[i] = server.arg(key).toInt();
            Serial.print(key + ": ");
            Serial.print(values[i]);
        } else {
            Serial.print("Missing: ");
            Serial.print(key);
            allFound = false;
        }
    }

    if (allFound) {
        msg = "POS001, 0";
        for (int i = 0; i < 8; i++){
            msg += ",";
            msg += String(values[i]);
        }
        process_communication_msgs("WiFi",msg);
    }

    server.send(200, "text/plain", "OK");
}

void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

void handlePose() {
    cmd = {2, 0, 500, 0}; xQueueSend(led_cmd_Queue, &cmd, 0);

    if (server.hasArg("name")) {
        String name = server.arg("name");
        
        String msg = "GRIP01,"+name;

        if (name == "open" || name == "fist" || name == "point" || name == "pinch" || name == "3pinch" || name == "cilinder" || name == "small_cilinder" || name == "relax"){
            process_communication_msgs("WiFi",msg);
        }else{
            errMsg = "[ Erro ] WiFi - Undefined pose name";
            Serial.println(errMsg);
        }
        
    }
    server.send(200, "text/plain", "OK");
}