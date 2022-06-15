#ifndef ESP32_WIFI_H
#define ESP32_WIFI_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define MAX_RETRY  20

#define WIFI_SSID "test"
#define WIFI_PASS "12345678"


void WifiEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

void WifiInitSta(void);



#endif