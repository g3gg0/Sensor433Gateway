#pragma once

#include <Arduino.h>
#include <time.h>

enum statusType
{
    Idle,
    Sent,
    Received,
    Pause
};

void ntp_setup();
bool ntp_loop();

const char *Time_getStateString();
void getTimeAdv(struct tm *tm, unsigned long offset);
void getTime(struct tm *tm);
void getStartupTime(struct tm *tm);
void sendNTPpacket(IPAddress &address);
