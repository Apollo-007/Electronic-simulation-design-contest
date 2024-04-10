#ifndef __AT24C02_H
#define __AT24C02_H

#include "main.h"
#include "gpio.h"

#define AT24C02_ADDR 0xa0

#define at24c02_port GPIOA
#define SCL_Pin GPIO_PIN_13
#define SDA_Pin GPIO_PIN_14

void at24c02_init();
void AT24C02_Write(uint8_t DatAddr, uint8_t Dat);
uint8_t AT24C02_Read(uint8_t DatAddr);

#endif
