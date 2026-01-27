#include <Arduino.h>

#include <Looper.h>
#include <RunningGFX.h>
#include <WiFiConnector.h>
#include <Wire.h>
TwoWire WIRE1 (0);
#include <iarduino_RTC.h>
#include <iarduino_I2C_Software.h>	


iarduino_RTC time_rtc(RTC_DS3231);

//iarduino_RTC time_rtc(RTC_DS1302, 25, 27, 26);  //DS1302, RST, CLK, DAT.



#include <GyverNTP.h>
#include "config.h"
#include "matrix.h"
#include "settings.h"


String str_proverka = "";

void runString(String str) {
    RunningGFX run(matrix);
    matrix.clear();
    matrix.setModeDiag();
    run.setSpeed(10);
    run.setWindow(0, matrix.width(), 1);
    run.setColor24(db[kk::clock_color]);
    run.setFont(gfx_font_3x5);
    run.setText(str);
    run.start();

    while (run.tick() != RG_FINISH) {
        delay(1);
        yield();
    }
}


void setup() {
    Serial.begin(115200);
    Serial.println("\n" PROJECT_NAME " v" PROJECT_VER);
    
    matrix.begin();
    time_rtc.begin(&WIRE1);
    
    WiFiConnector.setName(PROJECT_NAME);

    WiFiConnector.onConnect([]() {
        NTP.begin();

        Serial.print("Connected: ");
        Serial.println(WiFi.localIP());
        if (db[kk::show_ip]) runString(WiFi.localIP().toString());
        Serial.println(NTP.timeToString());
        Serial.println("Вайфай коннектор");
        //delay(5000);
        //time_rtc.settime(NTP.second(), NTP.minute(), NTP.hour(), NTP.day(), NTP.month(), NTP.year(), NTP.weekDay());
        //ota.checkUpdate();//Отключил обновление при старте
    });

    WiFiConnector.onError([]() {
        String str("noWifi");
        //str += WiFi.softAPSSID();
        Serial.println(str);
        runString(str);
    });


    LP_TIMER(1111, []() {
       if (db[kk::run_str_]) {
        if (db[kk::run_str_in].length() > 3) {
            str_proverka = db[kk::run_str_in].toString();
            str_proverka.replace("\n", " ");
            //db[kk::run_str_in] = db[kk::run_str_in].replace("\n", "");
            runString(str_proverka);
            
        }
        
        db[kk::run_str_] = false;
        //db[kk::run_str_in] = "";
    }
    });
    
}



void loop() {
    
    //Serial.println("Основной loop");
    Looper.loop();

    
    
}
