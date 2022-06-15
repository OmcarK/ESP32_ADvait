#include <stdio.h>
#include "ESP32_GPIO.h"

#define ESP_INTR_FLAG_DEFAULT 0



void GpioInit(void)
{    
ZCD_Triggered = false;

    gpio_config_t IoConfig;
    IoConfig.intr_type = GPIO_INTR_NEGEDGE;
    IoConfig.mode = GPIO_MODE_INPUT;
    IoConfig.pin_bit_mask = ZCD_INPUT_PIN_SEL;
    IoConfig.pull_down_en = 0;
    IoConfig.pull_up_en = 0; /* Change if required */
    gpio_config(&IoConfig);
    gpio_set_intr_type(ZCD_INPUT, GPIO_INTR_NEGEDGE);


#if 0
    IoConfig.intr_type = GPIO_INTR_DISABLE;  //disable interrupt
    IoConfig.mode = GPIO_MODE_OUTPUT;    //set as output mode
    IoConfig.pin_bit_mask = RLY_6_PIN_SEL; //disable pull-down mode
    IoConfig.pull_down_en = 0;   //disable pull-down mode
    IoConfig.pull_up_en = 0; //disable pull-up mode
    gpio_config(&IoConfig);  //configure GPIO with the given settings
#endif

/* Initializ Pin As OUTPUT  */
    IoConfig.intr_type = GPIO_INTR_DISABLE;
    IoConfig.mode = GPIO_MODE_OUTPUT;
    IoConfig.pin_bit_mask = RLY_4_PIN_SEL;
    IoConfig.pull_down_en = 0;
    IoConfig.pull_up_en = 0;
    gpio_config(&IoConfig);
 
    //gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
   // gpio_isr_handler_add(RLY_4, gpio_isr_handler, (void*) RLY_4);

/* Initializ Pin As OUTPUT  */
    IoConfig.intr_type = GPIO_INTR_DISABLE;
    IoConfig.mode = GPIO_MODE_OUTPUT;
    IoConfig.pin_bit_mask = RLY_3_PIN_SEL;
    IoConfig.pull_down_en = 0;
    IoConfig.pull_up_en = 0;
    gpio_config(&IoConfig);

/* Initializ Pin As OUTPUT */
    IoConfig.intr_type = GPIO_INTR_DISABLE;
    IoConfig.mode = GPIO_MODE_OUTPUT;
    IoConfig.pin_bit_mask = RLY_2_PIN_SEL;
    IoConfig.pull_down_en = 0;
    IoConfig.pull_up_en = 0;
    gpio_config(&IoConfig);

/* Initializ Pin  As OUTPUT */
    IoConfig.intr_type = GPIO_INTR_DISABLE;
    IoConfig.mode = GPIO_MODE_OUTPUT;
    IoConfig.pin_bit_mask = RLY_1_PIN_SEL;
    IoConfig.pull_down_en = 0;
    IoConfig.pull_up_en = 0;
    gpio_config(&IoConfig);
}
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    gpio_set_level(RLY_4,1);  /* Turn of AC  */
    ZCD_Triggered = true;
}