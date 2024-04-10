#ifndef  __AT24C02_H
#define  __AT24C02_H

#include "main.h"
#include "gpio.h"

#define AT24C02_ADDR 0xa0

#define at24c02_port GPIOA
#define SCL_Pin GPIO_PIN_14
#define SDA_Pin GPIO_PIN_15

void AT24C02_Init(void);
void delay1(unsigned int n);

void I2CStart(void);
void I2CStop(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
unsigned char I2CWaitAck(void);

void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);

void AT24C02_Read(unsigned char* pucBuf, unsigned char ucAddr, unsigned char ucNum);
void AT24C02_Write(unsigned char* pucBuf, unsigned char ucAddr, unsigned char ucNum);

#endif
