#include <map>
#include <list>
#include <string.h>

#ifndef RF_MODULE_FREQUENCY
#define RF_MODULE_FREQUENCY 433.92
#endif

#define JSON_MSG_BUFFER 512

char messageBuffer[JSON_MSG_BUFFER];

typedef struct map_entry
{
    char *first;
    char *second;
} t_map_entry;

std::list<t_map_entry> ReceiverLastReceived;

rtl_433_ESP rf(-1); // use -1 to disable transmitter


void rcv_addreceived(const char *path, const char *json)
{
    for (auto it = ReceiverLastReceived.begin(); it != ReceiverLastReceived.end(); it++)
    {
        /* entry with that path already found, just update json */
        if (!strcmp(it->first, path))
        {
            free(it->second);
            it->second = strdup(json);
            return;
        }
    }

    /* non found, add new one */
    t_map_entry entry;
    entry.first = strdup(path);
    entry.second = strdup(json);
    ReceiverLastReceived.push_back(entry);
}

void logJson(JsonObject &jsondata)
{
    char JSONmessageBuffer[jsondata.measureLength() + 1];
    jsondata.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.printf("Received message : %s" CR, JSONmessageBuffer);
}

void publish_string(const char *path, const char *elem, const char* value)
{
    char tmp[128];
    char topic[128];

    sprintf(tmp, "%s/%s", path, elem);
    strcpy(topic, "feeds/string/%s/");
    strcat(topic, tmp);

    mqtt_publish_string(topic, value);
}

void publish_float(const char *path, const char *elem, float value)
{
    char tmp[128];
    char topic[128];

    sprintf(tmp, "%s/%s", path, elem);
    strcpy(topic, "feeds/float/%s/");
    strcat(topic, tmp);

    mqtt_publish_float(topic, value);
}

void publish_int(const char *path, const char *elem, int value)
{
    char tmp[128];
    char topic[128];

    sprintf(tmp, "%s/%s", path, elem);
    strcpy(topic, "feeds/integer/%s/");
    strcat(topic, tmp);

    mqtt_publish_int(topic, value);
}

bool rcv_enabled(const char *path)
{
    char *fields = current_config.mqtt_filter;
    int pos = 0;

    while(fields[pos])
    {
        if(fields[pos] == ' ')
        {
            pos++;
        }
        else
        {
            const char *cur_str = &fields[pos];

            const char *end = strchr(cur_str, ' ');
            int length = strlen(cur_str);
            
            if(end)
            {
                length = (int)(end - cur_str);
            }

            if(!strncmp(cur_str, path, length))
            {
                return true;
            }

            pos += length;
        }
    }

    return false;
}

void rtl_433_Callback(char *msg)
{
    const char *message = strdup(msg);

    DynamicJsonBuffer json(JSON_MSG_BUFFER);
    JsonObject &data = json.parseObject(message);

    logJson(data);

    char path[128];
    const char* model = data["model"];
    int channel = data["channel"];

    bool publish = false;

    if(!strcmp(model, "status"))
    {
        sprintf(path, "status");
        publish = (current_config.mqtt_publish & 2);
    }
    else
    {
        sprintf(path, "%s-%d", model, channel);
        for(int pos = 0; pos < strlen(path); pos++)
        {
            if(path[pos] == ' ')
            {
                path[pos] = '_';
            }
        }

        rcv_addreceived(path, msg);

        publish = rcv_enabled(path) && (current_config.mqtt_publish & 1);
    }

    if(publish)
    {
        for (auto dataobj : data)
        {
            if(dataobj.value.is<int>())
            {
                publish_int(path, dataobj.key, dataobj.value);
            }
            else if(dataobj.value.is<float>())
            {
                publish_float(path, dataobj.key, dataobj.value);
            }
            else if(dataobj.value.is<bool>())
            {
                publish_int(path, dataobj.key, dataobj.value ? 1 : 0);
            }
            else if(dataobj.value.is<const char*>())
            {
                publish_string(path, dataobj.key, dataobj.value);
            }
        }
    }

    free((void*)message);
}

void rcv_setup()
{
    rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
    rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
    rf.enableReceiver(RF_MODULE_RECEIVER_GPIO);
}

bool rcv_loop()
{
    uint32_t time = millis();
    static int nextTime = 0;

    rf.loop();

    if (time >= nextTime)
    {
        nextTime = time + 60000;
        rf.getStatus(0);
    }
    return false;
}
