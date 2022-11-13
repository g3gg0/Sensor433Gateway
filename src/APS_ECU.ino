
#include "APS_ECU.h"

uint8_t aps_ecuid[64];

float aps_energy_lifetime = 0;
float aps_power_current = 0;
float aps_energy_day = 0;
float aps_frequency = 0;
float aps_temperature = 0;
float aps_power_a = 0;
float aps_voltage_a = 0;
float aps_power_b = 0;
float aps_voltage_b = 0;


void aps_setup()
{
}


bool aps_request(uint32_t command, uint8_t *payload, uint32_t payload_length, uint8_t *response, uint32_t *length)
{
    uint32_t time = millis();
    WiFiClient aps_client;

    aps_publish_string((char *)"status", "connecting");  
    Serial.printf("[APS] connecting to %s\n", current_config.aps_hostname);
    if (!aps_client.connect(current_config.aps_hostname, 8899))
    {
        aps_publish_string((char *)"status", "connection failed");  
        Serial.println("[APS] connection failed");
        return false;
    }
    Serial.printf("[APS] connected\n");
    aps_client.setTimeout(100);
    aps_client.setNoDelay(true);


    uint8_t buffer[128];
    uint32_t request_len = 3+2+4+4 + payload_length + 3;

    sprintf((char *)buffer, "APS%02d%04d%04d", 11, request_len, command);
    if(payload && payload_length > 0) 
    {
        memcpy(&buffer[13], payload, payload_length);
    }
    memcpy(&buffer[13+payload_length], "END", 3);

    buffer[request_len] = 0;
    
    Serial.printf("[APS] sending: '%s'\n", (char*)buffer);
    aps_client.write(buffer, request_len);
    aps_client.flush();

    while(aps_client.available() < 5) 
    {
        if(millis() - time > 1000)
        {
            aps_publish_string((char *)"status", "timeout in header receive");  
            Serial.println("[APS] timeout in header receive");
            aps_client.stop();
            return false;
        }
    }

    memset(buffer, 0x00, sizeof(buffer));

    if(aps_client.read(buffer, 5) != 5)
    {
        aps_publish_string((char *)"status", "no header received");  
        Serial.println("[APS] no header received");
        aps_client.stop();
        return false;
    }

    if(memcmp(buffer, "APS11", 5))
    {
        aps_publish_string((char *)"status", "no APS11 header");  
        Serial.println("[APS] no APS11 header");
        aps_client.stop();
        return false;
    }

    uint32_t response_length = 0;
    uint32_t response_command = 0;
    
    if(aps_client.read(buffer, 4) != 4)
    {
        aps_publish_string((char *)"status", "no length received");  
        Serial.println("[APS] no length received");
        aps_client.stop();
        return false;
    }
    buffer[4] = 0;
    sscanf((char *)buffer, "%04lu", &response_length);

    if(aps_client.read(buffer, 4) != 4)
    {
        aps_publish_string((char *)"status", "no command received"); 
        Serial.println("[APS] no command received");
        aps_client.stop();
        return false;
    }
    buffer[4] = 0;
    sscanf((char *)buffer, "%04lu", &response_command);

    *length = response_length - 16;

    if(aps_client.read(buffer, *length + 3) != *length + 3)
    {
        aps_publish_string((char *)"status", "no payload received"); 
        Serial.println("[APS] no payload received");
        aps_client.stop();
        return false;
    }

    memcpy(response, buffer, *length);

    aps_client.stop();

    return true;
}

void aps_publish_float(const char *name, float value)
{
    char path_buffer[128];

    if(strlen(current_config.aps_mqttpath) > 0)
    {
        sprintf(path_buffer, "feeds/float/%s/%s", current_config.aps_mqttpath, name);

        mqtt_publish_float_plain(path_buffer, value);
    }
}

void aps_publish_string(const char *name, const char * value)
{
    char path_buffer[128];

    if(strlen(current_config.aps_mqttpath) > 0)
    {
        sprintf(path_buffer, "feeds/string/%s/%s", current_config.aps_mqttpath, name);

        mqtt_publish_string_plain(path_buffer, value);
    }
}

bool aps_fetch()
{
    Serial.println("[APS] requesting general info");
    uint8_t buffer[128];
    uint32_t length = 0;

    if(!aps_request(1, NULL, 0, buffer, &length))
    {
        Serial.println("[APS] failed");
        return false;
    }

    if(length < 78)
    {
        aps_publish_string((char *)"status", "small payload received 1"); 
        Serial.printf("[APS] payload with only %d bytes\n", length);
        return false;
    }

    t_ecuinfo* infos = (t_ecuinfo *)buffer;

    aps_energy_lifetime = htonl(infos->energy_lifetime) * 100.0f;
    aps_power_current = htonl(infos->power_current);
    aps_energy_day = htonl(infos->energy_day) * 10.0f;

    memcpy(aps_ecuid, infos->ecuid, 12);
    aps_ecuid[12] = 0;

    Serial.printf("  ECU-ID:          %s\n", aps_ecuid);
    Serial.printf("  energy_lifetime: %f Wh\n", aps_energy_lifetime);
    Serial.printf("  power_current:   %f W\n", aps_power_current);
    Serial.printf("  energy_day:      %f Wh\n", aps_energy_day);

    /* detailed request */
    Serial.println("[APS] requesting detailed info");
    
    if(!aps_request(2, aps_ecuid, 12, buffer, &length) )
    {
        Serial.println("[APS] failed");
        return false;
    }
    if(length < 34)
    {
        aps_publish_string((char *)"status", "small payload received 2"); 
        Serial.printf("[APS] payload with only %d bytes\n", length);
        return false;
    }

    t_ecudetailed* detailed = (t_ecudetailed*)buffer;

    aps_frequency = htons(detailed->inverter.frequency) / 10.0f;
    aps_temperature = htons(detailed->inverter.temperature) - 100.0f;
    aps_power_a = htons(detailed->inverter.power_a);
    aps_voltage_a = htons(detailed->inverter.voltage_a) / 10.0f;
    aps_power_b = htons(detailed->inverter.power_b);
    aps_voltage_b = htons(detailed->inverter.voltage_b) / 10.0f;

    Serial.printf("  frequency:    %f Hz\n", aps_frequency);
    Serial.printf("  temperature:  %f °C\n", aps_temperature);
    Serial.printf("  power_a:      %f W\n", aps_power_a);
    Serial.printf("  voltage_a:    %f V\n", aps_voltage_a);
    Serial.printf("  power_b:      %f W\n", aps_power_b);
    Serial.printf("  voltage_b:    %f V\n", aps_voltage_b);

    return true;
}

bool aps_loop()
{
    uint32_t time = millis();
    static int nextTime = 5000;

    if (time >= nextTime)
    {
        if(aps_fetch())
        {
            aps_publish_float((char *)"energy_lifetime", aps_energy_lifetime);   
            aps_publish_float((char *)"power_current", aps_power_current);   
            aps_publish_float((char *)"energy_day", aps_energy_day);   
            aps_publish_float((char *)"frequency", aps_frequency);   
            aps_publish_float((char *)"temperature", aps_temperature);
            aps_publish_float((char *)"power_a", aps_power_a);   
            aps_publish_float((char *)"voltage_a", aps_voltage_a);   
            aps_publish_float((char *)"power_b", aps_power_b);   
            aps_publish_float((char *)"voltage_b", aps_voltage_b);
        }
        nextTime = time + 5*60000;
    }


    return false;
}
