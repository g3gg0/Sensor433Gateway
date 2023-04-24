#pragma once

void led_setup();
void led_set_adv(uint8_t n, uint8_t r, uint8_t g, uint8_t b, bool commit);
void led_set(uint8_t n, uint8_t r, uint8_t g, uint8_t b);
void led_set_all(uint8_t r, uint8_t g, uint8_t b);
void led_set_inhibit(bool state);
bool led_loop();
