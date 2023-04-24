#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <map>
#include <list>
#include <string.h>

typedef struct map_entry
{
    char *first;
    char *second;
} t_map_entry;

void rcv_addreceived(const char *path, const char *json);
void logJson(JsonObject &jsondata);
void publish_string(const char *path, const char *elem, const char *value);
void publish_float(const char *path, const char *elem, float value);
void publish_int(const char *path, const char *elem, int value);
bool rcv_enabled(const char *path);
void rtl_433_Callback(char *msg);
void rcv_setup();
bool rcv_loop();
