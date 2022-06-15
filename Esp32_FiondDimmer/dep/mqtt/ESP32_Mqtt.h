#ifndef ESP32MQTT_H
#define ESP32MQTT_H
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

#define MQTT_URL "mqtt://115.242.193.46"


extern uint8_t DutyPrcnt;

extern esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
extern void log_error_if_nonzero(const char * message, int error_code);
extern void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
extern void mqtt_app_start(void);
uint8_t StrToNum(char *StrPtr,uint8_t DataLen);
char* GetMacAddr(void);
char* GetMqttTopic(uint8_t OpNum);

#endif