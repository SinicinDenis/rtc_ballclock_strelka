#include "settings.h"

#include <GyverNTP.h>
#include <LittleFS.h>
#include <Looper.h>
#include <SettingsESP.h>
#include <WiFiConnector.h>
#include <iarduino_RTC.h>

#include "config.h"
#include "palettes.h"
#include "redraw.h"


//#include "main.cpp"


extern iarduino_RTC time_rtc;

AutoOTA ota(PROJECT_VER, PROJECT_URL);

GyverDBFile db(&LittleFS, "/data.db");

static SettingsESP sett(PROJECT_NAME " v" PROJECT_VER, &db);

static void update(sets::Updater& u) {
    String s;
    s += photo.getRaw();
    s += " [";
    s += db[kk::adc_min].toInt16();
    s += ", ";
    s += db[kk::adc_max].toInt16();
    s += "]";
    u.update("adc_val"_h, s);
    if (db[kk::time_ntp]) {
        u.update("local_time"_h, NTP.timeToString());
    } else {
        u.update("local_time"_h, time_rtc.gettime("H:i:s"));
    }
    u.update("synced"_h, NTP.synced());
    if (NTP.synced()) {
        if (millis() % 10000 > 9000) {
            //Serial.println('Установка времени rtc');
            time_rtc.settime(NTP.second(), NTP.minute(), NTP.hour(), NTP.day(), NTP.month(), NTP.year(), NTP.weekDay());
        } 
        
    }

    //if (db[kk::run_str_]) {
    //    runString(db[kk::run_str_in]);
    //}
    
    
    //Serial.println(time_rtc.gettime("H:i:s"));
    //Serial.println(NTP.timeToString());
    //Serial.println(WiFi.localIP());

    // Установка времени вручную ЧАС
    
    
    if (db[kk::rtc_set_h] != 0 && db[kk::rtc_set_m] == 0) {
        Serial.print("Час !!");
        Serial.println(db[kk::rtc_set_h]);
        time_rtc.settime(time_rtc.seconds,time_rtc.minutes,db[kk::rtc_set_h].toInt(),25,1,2025,1);
        db[kk::rtc_set_h] = 0;
    }
    // Установка времени вручную МИН
    if (db[kk::rtc_set_m] != 0 && db[kk::rtc_set_h] == 0) {
        Serial.print("Мин ");
        Serial.println(db[kk::rtc_set_m]);
        time_rtc.settime(time_rtc.seconds,db[kk::rtc_set_m].toInt(),time_rtc.Hours,25,1,2025,1);
        db[kk::rtc_set_m] = 0;
    }
    //Оба параметра и час и мин при включении
    if (db[kk::rtc_set_m] != 0 && db[kk::rtc_set_h] != 0) {
        Serial.println("Час Мин");
        time_rtc.settime(time_rtc.seconds,db[kk::rtc_set_m].toInt(),db[kk::rtc_set_h].toInt(),25,1,2025,1);
        db[kk::rtc_set_m] = 0;
        db[kk::rtc_set_h] = 0;
    }
    db.update();
    //if (ota.hasUpdate()) u.update("ota_update"_h, F("Доступно обновление. Обновить прошивку?"));

    Looper.getTimer("redraw")->restart(100);
}


static void build(sets::Builder& b) {
    {
        sets::Group g(b, "Часы");
        if (b.Switch(kk::cl_setup, "Часы ⚙️")) b.reload();
        if (db[kk::cl_setup]) {
            b.Select(kk::clock_style, "Шрифт", "Нет;Тип 1;Тип 2;Тип 3;Свой");
            b.Color(kk::clock_color, "Цвет");
            b.Switch(kk::clock_random, "Случайный цвет");
            b.Slider(kk::run_str_speed, "Скорость", 1, 5);
            
        }
    }
    {
        sets::Group g(b, "Фон");
        if (b.Switch(kk::fon_setup, "Фон ⚙️")) b.reload();

        if (db[kk::fon_setup]) {

            if (b.Select(kk::back_mode, "Фон", "Нет;Градиент;Перлин")) b.reload();

            if (db[kk::back_mode].toInt()) {
                b.Select(kk::back_pal, "Палитра", getPaletteList());
                b.Slider(kk::back_bright, "Яркость", 0, 255);
                b.Slider(kk::back_speed, "Скорость");
                b.Slider(kk::back_scale, "Масштаб", 0, 150);
                b.Slider(kk::back_angle, "Угол", -180, 180);
            }
        }
    }
    {
        sets::Group g(b, "Яркость");
        if (db[kk::auto_bright] == false) {b.Slider(kk::bright, "Яркость", 0, 180);}
        if (b.Switch(kk::auto_bright, "Автояркость")) b.reload();

        if (db[kk::auto_bright]) {
            b.Label("adc_val"_h, "Сигнал с датчика");
            b.Slider(kk::bright_min, "Мин.", 0, 180);
            b.Slider(kk::bright_max, "Макс.", 0, 180);

            {
                sets::Buttons bt(b);
                if (b.Button(kk::adc_min, "Запомнить мин.")) db[kk::adc_min] = photo.getRaw();
                if (b.Button(kk::adc_max, "Запомнить макс.")) db[kk::adc_max] = photo.getRaw();
            }
         
        }
    }
    {
        sets::Group g(b, "Ночной режим");

        if (b.Switch(kk::night_mode, "Включен")) b.reload();

        if (db[kk::night_mode]) {
            b.Color(kk::night_color, "Цвет");
            b.Slider(kk::night_trsh, "Порог", 0, 1023);
        }
    }
    {
        sets::Group g(b, "Время");
        if (b.Switch(kk::cl_menu, "Время ⚙️")) b.reload();
        if (db[kk::cl_menu]) {

            if (b.Switch(kk::time_ntp, "Время из интернета")) b.reload();
            //b.Label("adc_val"_h, "Сигнал с датчика");

            if (db[kk::time_ntp]) {

                b.Input(kk::ntp_gmt, "Часовой пояс");
                b.Input(kk::ntp_host, "NTP сервер");
                b.LED("synced"_h, "Синхронизирован", NTP.synced());
                b.Label("local_time"_h, "Интернет время", NTP.timeToString());

            } else {
                
                b.Label("local_time"_h,"Внутреннее время", time_rtc.gettime("H:i:s"));
                //Serial.println(time_rtc.gettime("H:i:s"));
                b.Input(kk::rtc_set_h, "Установить Час");
                b.Input(kk::rtc_set_m, "Установить Минуты");
                b.LED("synced"_h, "Синхронизирован", NTP.synced());
                
            }
        }
    }
    {   
        
        sets::Group g(b, "WiFi");
        if (b.Switch(kk::wifi_setup, "WiFi ⚙️ ")) b.reload();
        if (db[kk::wifi_setup]) {
            b.Switch(kk::show_ip, "Показывать ip -"+ WiFi.localIP().toString());
            b.Input(kk::wifi_ssid, "Название сети Wifi");
            b.Pass(kk::wifi_pass, "Пароль от Wifi", "");
            if (b.Button("wifi_save"_h, "Подключить")) {
                Looper.pushEvent("wifi_connect");
            }

        }
    }
    {
        sets::Group g(b, "Бегущая строка");
        if (b.Switch(kk::run_str_, " Запуск Строки ▶")) b.reload();
        b.Input(kk::run_str_in, "Ввод строки");
        
    }
    if (b.Confirm("ota_update"_h)) {
        if (b.build.value.toBool()) {
            Serial.println("OTA update!");
            ota.update();
        }
    }

    if (b.build.isAction()) {
        switch (b.build.id) {
            case kk::ntp_gmt: NTP.setGMT(b.build.value); break;
            case kk::ntp_host: NTP.setHost(b.build.value); break;
        }
    }

    Looper.getTimer("redraw")->restart(100);
    // if (b.Button("restart"_h, "restart")) ESP.restart();
}

LP_LISTENER_("wifi_connect", []() {
    db.update();
    WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
});

LP_TICKER([]() {
    if (Looper.thisSetup()) {
        LittleFS.begin(true);
        db.begin();


        db.init(kk::rtc_set_h, time_rtc.Hours);//Переменная для установки времени Час
        db.init(kk::rtc_set_m, time_rtc.minutes);//Переменная для установки времени Минуты
        db.init(kk::fon_setup, false);//переменная фон для меню
        db.init(kk::time_ntp, false);//Переменная для меню времени
        db.init(kk::wifi_setup, false);//Переменная меню вайфай
        db.init(kk::time_rtc_, false);//переменная время rtc
        db.init(kk::cl_setup, false);//переменная меню время
        db.init(kk::cl_menu, false);//переменная меню выезжающее время
        db.init(kk::clock_random, false);
        db.init(kk::wifi_ssid, "");
        db.init(kk::wifi_pass, "");
        db.init(kk::show_ip, true);

        db.init(kk::run_str_, false);
        db.init(kk::run_str_in, "");
        db.init(kk::run_str_speed, 1);//переменная для скорости смена цвета часов


        db.init(kk::ntp_host, "pool.ntp.org");
        db.init(kk::ntp_gmt, 3);

        db.init(kk::bright, 100);
        db.init(kk::auto_bright, false);
        db.init(kk::bright_min, 10);
        db.init(kk::bright_max, 255);
        db.init(kk::adc_min, 0);
        db.init(kk::adc_max, 1023);

        db.init(kk::night_mode, false);
        
        
        db.init(kk::night_color, 0xff0000);
        db.init(kk::night_trsh, 50);

        db.init(kk::clock_style, 1);
        db.init(kk::clock_color, 0xffffff);

        db.init(kk::back_mode, 1);
        db.init(kk::back_pal, 0);
        db.init(kk::back_bright, 200);
        db.init(kk::back_speed, 50);
        db.init(kk::back_scale, 50);
        db.init(kk::back_angle, 0);

        WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
        sett.begin();
        sett.onBuild(build);
        sett.onUpdate(update);

        NTP.setHost(db[kk::ntp_host]);
        NTP.setGMT(db[kk::ntp_gmt]);
    }

    WiFiConnector.tick();
    sett.tick();
    //ota.tick();
    NTP.tick();
    db.tick();
});

//LP_TIMER(24ul * 60 * 60 * 1000, []() {
//    ota.checkUpdate();
//});
byte rr=255, gg=255, bb=255;
int jj=0;

void setColorFromWheel(int wheelPos) {
    if (wheelPos < 85) {
        rr = 255 - wheelPos * 3;
        gg = wheelPos * 3;
        bb = 0;
    } else if (wheelPos < 170) {
        wheelPos -= 85;
        rr = 0;
        gg = 255 - wheelPos * 3;
        bb = wheelPos * 3;
    } else {
        wheelPos -= 170;
        rr = wheelPos * 3;
        gg = 0;
        bb = 255 - wheelPos * 3;
    }
}

long koleso() {
    for (jj < 768; jj+=db[kk::run_str_speed].toInt();) {
        setColorFromWheel(jj % 255);
        return long((rr << 16) | (gg << 8) | (bb));
        }
    return long((rr << 16) | (gg << 8) | (bb));
}



LP_TIMER(50, []() {
        if (db[kk::clock_random]) {
            db[kk::clock_color] = koleso();
        }
});


// LP_TIMER(1000, []() {
//     Serial.println(ESP.getFreeHeap());
// });