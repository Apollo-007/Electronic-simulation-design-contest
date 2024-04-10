#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"
#include "gpio.h"

#define DHT11_Port GPIOB
#define DHT11_Pin GPIO_PIN_13

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

//IO操作函数
#define	DHT11_DQ_OUT PAout(4) //数据端口	PA4
#define	DHT11_DQ_IN  PAin(4)  //数据端口	PAA

#define DHT11_Write(val) HAL_GPIO_WritePin(DHT11_Port,DHT11_Pin,(GPIO_PinState)(val))
#define DHT11_Read HAL_GPIO_ReadPin(DHT11_Port,DHT11_Pin)

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp, u8 *humi); //读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11
u8 DHT11_Read_Data_Float(float *temp,float *humi);


#endif
