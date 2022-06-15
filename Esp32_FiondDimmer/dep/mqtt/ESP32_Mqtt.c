#include <stdio.h>
#include <math.h>
#include "ESP32_Mqtt.h"

uint8_t ConnectFlag = 1;
static const char *TAG = "MQTT";
uint8_t DutyPrcnt;
char MacAddr[30];

typedef struct 
{
    char TopicStr[30];
}MqttTopic;

MqttTopic Topic[6];

char *OpTopicArr[6] = 
{
    "OP1",
    "OP2",
    "OP3",
    "OP4",
    "OP5",
    "OP6"
};

void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) 
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

 esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    char* DeviceInfoStr = NULL;
    cJSON* DeviceInfo = cJSON_CreateObject();    

    esp_mqtt_client_handle_t client = event->client;
    int msg_id=0;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
                if(ConnectFlag)
                {
                    cJSON_AddStringToObject(DeviceInfo,"MAC_ID",GetMacAddr());
                    cJSON_AddNumberToObject(DeviceInfo,"INPUT",0);
                    cJSON_AddNumberToObject(DeviceInfo,"OUTPUT",1);
                    cJSON_AddStringToObject(DeviceInfo,"DEVICE_NAME","DIMMER");
                    cJSON_AddStringToObject(DeviceInfo,"DEVICE_TYPE","HOME_AUTOMATION");
                    DeviceInfoStr = cJSON_Print(DeviceInfo);
                    cJSON_Minify(DeviceInfoStr);
                    cJSON_Delete(DeviceInfo);
                    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
                    msg_id = esp_mqtt_client_publish(client,"DEVICE_INFO/DIMMER",DeviceInfoStr,0,0,0);
                    ConnectFlag=0;
                }
                msg_id = esp_mqtt_client_subscribe(client,GetMqttTopic(0), 0);
                cJSON_free(DeviceInfoStr);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");    
            DutyPrcnt = StrToNum(event->data,event->data_len);
            printf("DutyCucle=%d\n",DutyPrcnt);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start()
{
    esp_mqtt_client_config_t mqtt_cfg = {
       .uri = MQTT_URL,
       //.uri = "mqtt://192.168.0.109",
       //.uri = "mqtt://test.mosquitto.org",
    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

uint8_t StrToNum(char *StrPtr,uint8_t DataLen)
{
    uint8_t Indx=0;
	uint8_t Cntr=0;
	uint8_t NumIndx=0;
    uint8_t Num[10]={};
    uint8_t TempDuty=0;
		while(Indx < DataLen)
		{	
            Num[Cntr] = *StrPtr - '0';
            Cntr++; 
                if(Indx == DataLen-1)
                {
                    NumIndx=0;
                    while(Cntr>0)
                    {
                        TempDuty = TempDuty + Num[NumIndx] * ((int)pow(10,Cntr-1));
                        Cntr--;
                        NumIndx++;
                    }
                }
                StrPtr++;
				Indx++;	
        }
    return TempDuty;
}

char* GetMacAddr()
{
    uint8_t TempMacAddr[6];
    esp_read_mac(TempMacAddr,ESP_MAC_WIFI_STA);
    sprintf(MacAddr,"%02x%02x%02x%02x%02x%02x",TempMacAddr[0],TempMacAddr[1],TempMacAddr[2],
                                                TempMacAddr[3],TempMacAddr[4],TempMacAddr[5]);
    return MacAddr;
}

char* GetMqttTopic(uint8_t OpNum)
{   
    strcpy(Topic[OpNum].TopicStr,"DIMMER/");
    strcat(Topic[OpNum].TopicStr,MacAddr);
    strcat(Topic[OpNum].TopicStr,"/");
    strcat(Topic[OpNum].TopicStr,OpTopicArr[OpNum]);
    return Topic[OpNum].TopicStr;
}