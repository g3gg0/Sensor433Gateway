
#include <Arduino.h>
#include <LED.h>

#define LED_GPIO ONBOARD_LED

bool led_inhibit = false;

void led_setup()
{
}

void led_set_adv(uint8_t n, uint8_t r, uint8_t g, uint8_t b, bool commit)
{
}

void led_set(uint8_t n, uint8_t r, uint8_t g, uint8_t b)
{
    digitalWrite(LED_GPIO, ((r > 0) || (g > 0) || (b > 0)) ? HIGH : LOW);
}

void led_set_all(uint8_t r, uint8_t g, uint8_t b)
{
}

void led_set_inhibit(bool state)
{
    led_inhibit = state;
}

bool led_loop()
{
    return false;
}
