
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/timer.h"
#include "../dep/mqtt/ESP32_Mqtt.h"
#include "../dep/wifi/ESP32_Wifi.h"
#include "../dep/Gpio/ESP32_GPIO.h"

#define TIMER_INTR_SEL TIMER_INTR_LEVEL
#define TIMER_GROUP TIMER_GROUP_0
#define TIMER_DIVIDER 80
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)
#define DIMMER_INC_FACT 2000

#define PRO_CPU 0
#define APP_CPU 1

void NvsFlashInit(void);
void IRAM_ATTR TmrIsr(void *Arg);
void TmrInit(void);

uint32_t Cntr = 1;
uint8_t DiffCntr;

static volatile uint16_t prevdutyPercent; 
/* this will store the previous duty percent value.
* When we get new dutypercent this value will incremented gradullay according in DIMMER_INC_FACT value */

static volatile uint16_t dutyPercentIncContr;

typedef struct
{
    const uint8_t LookUpArr[8];
    uint8_t State;
    uint8_t Indx;
    uint8_t FinalIn;
    uint8_t PrivFinalIn;
    uint8_t FallingEdge;
    uint8_t PrevFallingEdge;
} Zcd;

Zcd ZcdParam = {{0, 0, 0, 1, 0, 1, 1, 1},0,0,0,0,0,0};
xTaskHandle TmrTaskHandle;

void TmrTask(void *PvParameter)
{
    TmrInit();

}

void app_main(void)
{
    portDISABLE_INTERRUPTS();
    DutyPrcnt = 0;
    GpioInit();
    TmrInit();
    NvsFlashInit();
    WifiInitSta();
    portENABLE_INTERRUPTS();
   // xTaskCreatePinnedToCore(TmrTask, "TmrTask", 2048, NULL, 10, &TmrTaskHandle, APP_CPU);
   while (1)
   {
    vTaskDelay(10);
   }
   
}

void TmrInit()
{
    int TmrGrp = TIMER_GROUP;
    int TmrIdx = TIMER_0;

    timer_config_t TmrConfig;
    TmrConfig.alarm_en = 1;
    TmrConfig.auto_reload = 1;
    TmrConfig.counter_dir = TIMER_COUNT_UP;
    TmrConfig.divider = TIMER_DIVIDER;
    TmrConfig.intr_type = TIMER_INTR_SEL;
    TmrConfig.counter_en = TIMER_PAUSE;

    timer_init(TmrGrp, TmrIdx, &TmrConfig);
    timer_pause(TmrGrp, TmrIdx);
    timer_set_counter_value(TmrGrp, TmrIdx, 0x00000000ULL);
    timer_set_alarm_value(TmrGrp, TmrIdx, 1);
    timer_enable_intr(TmrGrp, TmrIdx);
    timer_isr_register(TmrGrp, TmrIdx, TmrIsr, (void *)TmrIdx, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TmrGrp, TmrIdx);
}

void IRAM_ATTR TmrIsr(void *Arg)
{

    int TempTmrIdx = (int)Arg;
    uint32_t IntrStatus = TIMERG0.int_st_timers.val;
    volatile uint16_t PluseOfTime;

    if(DIMMER_INC_FACT != dutyPercentIncContr)
    {
        dutyPercentIncContr = dutyPercentIncContr+50;
    }
    else
    {
        if(prevdutyPercent < DutyPrcnt)
        {
            prevdutyPercent++;
            dutyPercentIncContr = 0;
        }
        else if(prevdutyPercent > DutyPrcnt)
        {
            prevdutyPercent--;
            dutyPercentIncContr = 0;
        }
        else
        {
            /* prevdutyPercent = DutyPrcnt; */
        }  
    }

    if(prevdutyPercent<30)prevdutyPercent=30;

    PluseOfTime = (prevdutyPercent - 30) * (142); /* using this formula values are exactly mapped from 30 to 100 to 0 to 10000 */
    /* https://rosettacode.org/wiki/Map_range */

    if ((IntrStatus & BIT(TempTmrIdx)) && TempTmrIdx == TIMER_0)
    {
        TIMERG0.hw_timer[TempTmrIdx].update = 1;
        TIMERG0.int_clr_timers.t0 = 1;
        TIMERG0.hw_timer[TempTmrIdx].config.alarm_en = 1;
    }

    if((Cntr == PluseOfTime) && (ZCD_Triggered == true))
    {
        gpio_set_level(RLY_4,0);
        Cntr = 1;
        ZCD_Triggered = false;
    }
    Cntr++;
}
void NvsFlashInit()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
}