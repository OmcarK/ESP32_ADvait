#ifndef ESP32_GPIO_H
#define ESP32_GPIO_H
#include "driver/gpio.h"

#define RLY_6       15
#define RLY_4       19
#define RLY_3       18
#define RLY_2       02
#define RLY_1       05

#define RLY_3_PIN_SEL   (1ULL<<RLY_3)
#define RLY_2_PIN_SEL   (1ULL<<RLY_2)
#define RLY_1_PIN_SEL   (1ULL<<RLY_1)

#define ZCD_INPUT   16  
#define USR_LED     23
#define USR_LED_PIN_SEL     (1ULL<<USR_LED)    
#define RLY_4_PIN_SEL       (1ULL<<RLY_4)
#define RLY_6_PIN_SEL       (1ULL<<RLY_6)
#define ZCD_INPUT_PIN_SEL   (1ULL<<ZCD_INPUT)
volatile uint8_t ZCD_Triggered;
extern void GpioInit(void);
#endif
