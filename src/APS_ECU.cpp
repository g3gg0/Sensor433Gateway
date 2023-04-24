
#include <Arduino.h>
#include <WiFi.h>
#include <APS_ECU.h>
#include <HA.h>
#include <Config.h>

float aps_energy_lifetime = 0;
float aps_power_current = 0;
float aps_energy_day = 0;
float aps_frequency = 0;
float aps_temperature = 0;
float aps_power_a = 0;
float aps_voltage_a = 0;
float aps_power_b = 0;
float aps_voltage_b = 0;
char aps_channel[3];
char aps_ecu_serial[32];
char aps_ecu_firmware[32];
char aps_inv_serial[32];
char aps_inv_unk[16];
char aps_inv_model[16];
char aps_timestamp[32];
char aps_timestamp_mqtt[32];

void mqtt_publish_string_plain(const char *path_buffer, const char *value);
void mqtt_publish_float_plain(const char *path_buffer, float value);
void mqtt_publish_int_plain(const char *path_buffer, uint32_t value);

void aps_setup()
{
    t_ha_entity entity;

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "channel";
    entity.name = "Channel";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/channel";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "power_current";
    entity.name = "Solar power";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/power_current";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "W";
    entity.dev_class = "power";
    entity.state_class = "measurement";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "energy_day";
    entity.name = "Solar energy daily";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/energy_day";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "Wh";
    entity.dev_class = "energy";
    entity.state_class = "total_increasing";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "energy_lifetime";
    entity.name = "Solar energy lifetime";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/energy_lifetime";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "Wh";
    entity.dev_class = "energy";
    entity.state_class = "total_increasing";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "power_a";
    entity.name = "Solar power channel A";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/power_a";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "W";
    entity.dev_class = "power";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "power_b";
    entity.name = "Solar power channel B";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/power_b";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "W";
    entity.dev_class = "power";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "voltage_a";
    entity.name = "Solar voltage channel A";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/voltage_a";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "V";
    entity.dev_class = "voltage";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "voltage_b";
    entity.name = "Solar voltage channel B";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/voltage_b";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "V";
    entity.dev_class = "voltage";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "frequency";
    entity.name = "Solar mains frequency";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/frequency";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "Hz";
    entity.dev_class = "frequency";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "temperature";
    entity.name = "Solar inverter temperature";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/float/%s/temperature";
    entity.alt_name = current_config.aps_mqttpath;
    entity.unit_of_meas = "°C";
    entity.dev_class = "temperature";
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "ecu_serial";
    entity.name = "Solar gateway serial number";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/ecu_serial";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "ecu_firmware";
    entity.name = "Solar gateway firmware";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/ecu_firmware";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "inverter_serial";
    entity.name = "Solar inverter serial number";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/inverter_serial";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "inverter_unknown";
    entity.name = "Solar inverter unknown info";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/inverter_unknown";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "inverter_model";
    entity.name = "Solar inverter model";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/inverter_model";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);

    memset(&entity, 0x00, sizeof(entity));
    entity.id = "timestamp";
    entity.name = "Solar gateway sync timestamp";
    entity.type = ha_sensor;
    entity.stat_t = "feeds/string/%s/timestamp";
    entity.alt_name = current_config.aps_mqttpath;
    ha_add(&entity);
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
    uint32_t request_len = 3 + 2 + 4 + 4 + payload_length + 3;

    sprintf((char *)buffer, "APS%02d%04d%04d", 11, request_len, command);
    if (payload && payload_length > 0)
    {
        memcpy(&buffer[13], payload, payload_length);
    }
    memcpy(&buffer[13 + payload_length], "END", 3);

    buffer[request_len] = 0;

    Serial.printf("[APS] sending: '%s'\n", (char *)buffer);
    aps_client.write(buffer, request_len);
    aps_client.flush();

    while (aps_client.available() < 5)
    {
        if (millis() - time > 1000)
        {
            aps_publish_string((char *)"status", "timeout in header receive");
            Serial.println("[APS] timeout in header receive");
            aps_client.stop();
            return false;
        }
    }

    memset(buffer, 0x00, sizeof(buffer));

    if (aps_client.read(buffer, 5) != 5)
    {
        aps_publish_string((char *)"status", "no header received");
        Serial.println("[APS] no header received");
        aps_client.stop();
        return false;
    }

    if (memcmp(buffer, "APS11", 5))
    {
        aps_publish_string((char *)"status", "no APS11 header");
        Serial.println("[APS] no APS11 header");
        aps_client.stop();
        return false;
    }

    uint32_t response_length = 0;
    uint32_t response_command = 0;

    if (aps_client.read(buffer, 4) != 4)
    {
        aps_publish_string((char *)"status", "no length received");
        Serial.println("[APS] no length received");
        aps_client.stop();
        return false;
    }
    buffer[4] = 0;
    sscanf((char *)buffer, "%04lu", &response_length);

    if (aps_client.read(buffer, 4) != 4)
    {
        aps_publish_string((char *)"status", "no command received");
        Serial.println("[APS] no command received");
        aps_client.stop();
        return false;
    }
    buffer[4] = 0;
    sscanf((char *)buffer, "%04lu", &response_command);

    *length = response_length - 16;

    if (aps_client.read(buffer, *length + 3) != *length + 3)
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

void aps_publish_int(const char *name, uint32_t value)
{
    char path_buffer[128];

    if (strlen(current_config.aps_mqttpath) > 0)
    {
        sprintf(path_buffer, "feeds/integer/%s/%s", current_config.aps_mqttpath, name);

        mqtt_publish_int_plain(path_buffer, value);
    }
}

void aps_publish_float(const char *name, float value)
{
    char path_buffer[128];

    if (strlen(current_config.aps_mqttpath) > 0)
    {
        sprintf(path_buffer, "feeds/float/%s/%s", current_config.aps_mqttpath, name);

        mqtt_publish_float_plain(path_buffer, value);
    }
}

void aps_publish_string(const char *name, const char *value)
{
    char path_buffer[128];

    if (strlen(current_config.aps_mqttpath) > 0)
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

    if (!aps_request(1, NULL, 0, buffer, &length))
    {
        Serial.println("[APS] failed");
        return false;
    }

    if (length < 78)
    {
        aps_publish_string((char *)"status", "small payload received 1");
        Serial.printf("[APS] payload with only %d bytes\n", length);
        return false;
    }

    t_ecuinfo *infos = (t_ecuinfo *)buffer;

    aps_energy_lifetime = htonl(infos->energy_lifetime) * 100.0f;
    aps_power_current = htonl(infos->power_current);
    aps_energy_day = htonl(infos->energy_day) * 10.0f;
    sprintf(aps_channel, "%c%c", infos->ecu_channel[0], infos->ecu_channel[1]);

    memcpy(aps_ecu_serial, infos->ecuid, 12);
    aps_ecu_serial[12] = 0;

    Serial.printf("  ECU-ID:          %s\n", aps_ecu_serial);
    Serial.printf("  energy_lifetime: %f Wh\n", aps_energy_lifetime);
    Serial.printf("  power_current:   %f W\n", aps_power_current);
    Serial.printf("  energy_day:      %f Wh\n", aps_energy_day);

    char *strings = (char *)&infos[1];
    int version_len = 0;
    char buf[4];

    strncpy(buf, strings, 3);
    buf[3] = 0;
    version_len = atoi(buf);
    strncpy(aps_ecu_firmware, &strings[3], version_len);
    aps_ecu_firmware[version_len] = 0;
    Serial.printf("  ECU-Version:     %s\n", aps_ecu_firmware);

    /* detailed request */
    Serial.println("[APS] requesting detailed info");

    if (!aps_request(2, (uint8_t *)aps_ecu_serial, 12, buffer, &length))
    {
        Serial.println("[APS] failed");
        return false;
    }
    if (length < 34)
    {
        aps_publish_string((char *)"status", "small payload received 2");
        Serial.printf("[APS] payload with only %d bytes\n", length);
        return false;
    }

    t_ecudetailed *detailed = (t_ecudetailed *)buffer;

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

    strcpy(aps_inv_unk, "");
    for (int pos = 0; pos < sizeof(detailed->inverter.unknown); pos++)
    {
        char buf[3];
        sprintf(buf, "%02X", detailed->inverter.unknown[pos]);
        strcat(aps_inv_unk, buf);
    }
    Serial.printf("  unknown:      %s\n", aps_inv_unk);

    sprintf(aps_inv_model, "%02X", detailed->inverter.model);
    Serial.printf("  model:        0x%02X\n", detailed->inverter.model);

    strcpy(aps_timestamp, "");
    for (int pos = 0; pos < sizeof(detailed->inverter.timestamp); pos++)
    {
        char buf[3];
        sprintf(buf, "%02X", detailed->inverter.timestamp[pos]);
        strcat(aps_timestamp, buf);
    }

    strcpy(aps_timestamp_mqtt, "");
    strncat(aps_timestamp_mqtt, &aps_timestamp[0], 4);
    strcat(aps_timestamp_mqtt, "-");
    strncat(aps_timestamp_mqtt, &aps_timestamp[4], 2);
    strcat(aps_timestamp_mqtt, "-");
    strncat(aps_timestamp_mqtt, &aps_timestamp[6], 2);
    strcat(aps_timestamp_mqtt, " ");
    strncat(aps_timestamp_mqtt, &aps_timestamp[8], 2);
    strcat(aps_timestamp_mqtt, ":");
    strncat(aps_timestamp_mqtt, &aps_timestamp[10], 2);
    strcat(aps_timestamp_mqtt, ":");
    strncat(aps_timestamp_mqtt, &aps_timestamp[12], 2);

    Serial.printf("  timestamp:    %s\n", aps_timestamp_mqtt);

    strcpy(aps_inv_serial, "");
    for (int pos = 0; pos < sizeof(detailed->inverter.uid); pos++)
    {
        char buf[3];
        sprintf(buf, "%02X", detailed->inverter.uid[pos]);
        strcat(aps_inv_serial, buf);
    }
    Serial.printf("  uid:          %s\n", aps_inv_serial);

    return true;
}

bool aps_loop()
{
    uint32_t time = millis();
    static int nextTime = 5000;

    if (time >= nextTime)
    {
        if (aps_fetch())
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
            aps_publish_string((char *)"ecu_serial", aps_ecu_serial);
            aps_publish_string((char *)"ecu_firmware", aps_ecu_firmware);
            aps_publish_string((char *)"inverter_serial", aps_inv_serial);
            aps_publish_string((char *)"inverter_unknown", aps_inv_unk);
            aps_publish_string((char *)"inverter_model", aps_inv_model);
            aps_publish_string((char *)"timestamp", aps_timestamp_mqtt);
            aps_publish_string((char *)"channel", aps_channel);
        }
        nextTime = time + 5 * 60000;
    }

    return false;
}
