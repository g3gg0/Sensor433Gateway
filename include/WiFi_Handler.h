#pragma once

#include <Arduino.h>

#define WIFI_LED 1

void wifi_setup();
void wifi_off();
void wifi_enter_captive();
bool wifi_loop();
