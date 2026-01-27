#pragma once

#include <GyverDBFile.h>
#include <AutoOTA.h>
#include "matrix.h"

extern GyverDBFile db;
extern AutoOTA ota;
extern BallMatrix matrix;

DB_KEYS(
    kk,
    wifi_ssid,
    wifi_pass,
    show_ip,

    ntp_gmt,
    ntp_host,

    bright,
    auto_bright,
    bright_min,
    bright_max,
    adc_min,
    adc_max,

    night_mode,
    night_color,
    night_trsh,

    clock_style,
    clock_color,

    back_mode,  // 0 none, 1 grad, 2 perlin
    back_pal,
    back_speed,
    back_angle,
    back_scale,
    back_bright,
    slider_,
    time_ntp,
    time_rtc_,
    wifi_setup,
    fon_setup,
    rtc_set_h,
    rtc_set_m,
    cl_setup,
    cl_menu,
    clock_random,
    run_str_,
    run_str_in,
    run_str_speed
);