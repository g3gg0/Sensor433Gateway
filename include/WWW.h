#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <ESP32httpUpdate.h>

#include <Config.h>
#include <Receiver.h>

#define xstr(s) str(s)
#define str(s) #s

#define min(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define max(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

void www_setup();
bool www_loop();
void www_activity();
int www_is_captive_active();
void www_handle_404();
void www_handle_index();
void www_handle_root();
void www_handle_ota();
void www_handle_reset();
void www_handle_set_parm();
String www_send_html();
