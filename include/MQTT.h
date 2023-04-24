#pragma once

void MQTT_connect();
void mqtt_setup();
bool mqtt_loop();

void mqtt_publish_string(const char *name, const char *value);
void mqtt_publish_string_plain(const char *path_buffer, const char *value);
void mqtt_publish_float(const char *name, float value);
void mqtt_publish_float_plain(const char *path_buffer, float value);
void mqtt_publish_int(const char *name, uint32_t value);
void mqtt_publish_int_plain(const char *path_buffer, uint32_t value);
