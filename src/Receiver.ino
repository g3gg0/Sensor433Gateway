
#ifndef RF_MODULE_FREQUENCY
#define RF_MODULE_FREQUENCY 433.92
#endif

#define JSON_MSG_BUFFER 512

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf(-1); // use -1 to disable transmitter


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

void rtl_433_Callback(char *message)
{
    DynamicJsonBuffer json(JSON_MSG_BUFFER);
    JsonObject &data = json.parseObject(message);

    logJson(data);

    char path[128];
    const char* model = data["model"];
    int channel = data["channel"];  

    sprintf(path, "%s-%d", model, channel);

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

void rcv_setup()
{
    rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
    rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
    rf.enableReceiver(RF_MODULE_RECEIVER_GPIO);
}

bool rcv_loop()
{
    rf.loop();
    return false;
}
