#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "main.h"
#include "gpio.h"

#define keyboard_port GPIOA
#define col0_pin GPIO_PIN_0
#define col1_pin GPIO_PIN_1
#define col2_pin GPIO_PIN_2
#define col3_pin GPIO_PIN_3
#define row0_pin GPIO_PIN_4
#define row1_pin GPIO_PIN_5
#define row2_pin GPIO_PIN_6
#define row3_pin GPIO_PIN_7


void keyboard_init(void);
uint8_t key_scan(void);

#endif
