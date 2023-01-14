
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <SPIFFS.h>
#include <rtl_433_ESP.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#include <esp_task_wdt.h>
#define WDT_TIMEOUT 180

#include "Config.h"

int loopCount = 0;

float main_duration_avg = 0;
float main_duration = 0;
float main_duration_max = 0;
float main_duration_min = 1000000;

extern bool config_valid;


void setup()
{
    Serial.begin(115200);
    Serial.printf("\n\n\n");

    esp_task_wdt_reset();

    Serial.printf("[i] SDK:          '%s'\n", ESP.getSdkVersion());
    Serial.printf("[i] CPU Speed:    %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("[i] Chip Id:      %06llX\n", ESP.getEfuseMac());
    Serial.printf("[i] Flash Mode:   %08X\n", ESP.getFlashChipMode());
    Serial.printf("[i] Flash Size:   %08X\n", ESP.getFlashChipSize());
    Serial.printf("[i] Flash Speed:  %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
    Serial.printf("[i] Heap          %d/%d\n", ESP.getFreeHeap(), ESP.getHeapSize());
    Serial.printf("[i] SPIRam        %d/%d\n", ESP.getFreePsram(), ESP.getPsramSize());
    Serial.printf("\n");
    Serial.printf("[i] Starting\n");

    Serial.printf("[i]   Setup WDT\n");
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);

    Serial.printf("[i]   Setup LEDs\n");
    led_setup();
    Serial.printf("[i]   Setup SPIFFS\n");
    if (!SPIFFS.begin(true))
    {
        Serial.println("[E]   SPIFFS Mount Failed");
    }
    cfg_read();
    Serial.printf("[i]   Setup WiFi\n");
    wifi_setup();
    Serial.printf("[i]   Setup Webserver\n");
    www_setup();
    Serial.printf("[i]   Setup Time\n");
    time_setup();
    Serial.printf("[i]   Setup MQTT\n");
    mqtt_setup();
    Serial.printf("[i]   Setup rtl433\n");
    rcv_setup();
    Serial.printf("[i]   Setup APS ECU\n");
    aps_setup();

    Serial.println("Setup done");
}


void loop()
{
    bool hasWork = false;

    uint64_t startTime = micros();

    hasWork |= led_loop();
    hasWork |= wifi_loop();
    hasWork |= www_loop();
    hasWork |= time_loop();
    hasWork |= mqtt_loop();
    hasWork |= ota_loop();
    hasWork |= rcv_loop();
    hasWork |= aps_loop();

    uint64_t duration = micros() - startTime;

    main_duration = duration;
    main_duration_avg = (15 * main_duration_avg + duration) / 16.0f;

    if (main_duration < main_duration_min)
    {
        main_duration_min = main_duration;
    }
    if (main_duration > main_duration_max)
    {
        main_duration_max = main_duration;
    }

    loopCount++;

    if(!hasWork)
    {
        delay(100);
    }
    else
    {
        delay(10);
    }
    esp_task_wdt_reset();
}
