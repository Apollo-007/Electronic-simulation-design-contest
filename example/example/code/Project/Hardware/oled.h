#ifndef __OLED_H
#define __OLED_H 

#include "main.h"
#include "stdlib.h"	

//-----------------OLED端口定义---------------- 

//#define OLED_SCL_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET)//SCL
//#define OLED_SCL_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET)

//#define OLED_SDA_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)//SDA
//#define OLED_SDA_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)

//#define OLED_RES_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)//RES
//#define OLED_RES_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)

#define OLED_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_14
#define OLED_SDA_Pin GPIO_PIN_15

#define OLED_SCL_Clr() OLED_Port->BSRR = (uint32_t)OLED_SCL_Pin << 16u //SCL
#define OLED_SCL_Set() OLED_Port->BSRR = OLED_SCL_Pin

#define OLED_SDA_Clr() OLED_Port->BSRR = (uint32_t)OLED_SDA_Pin << 16u//SDA
#define OLED_SDA_Set() OLED_Port->BSRR = OLED_SDA_Pin

#define OLED_RES_Clr() OLED_Port->BSRR = (uint32_t)GPIO_PIN_2 << 16u//RES
#define OLED_RES_Set() OLED_Port->BSRR = GPIO_PIN_2


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

/*
0------------->x
|
|
|
|
\/
y
*/

void OLED_ClearPoint(uint8_t x,uint8_t y);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(uint8_t dat);
void OLED_WR_Byte(uint8_t dat,uint8_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void OLED_ShowChar6x8(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);
void OLED_Init(void);

#endif

