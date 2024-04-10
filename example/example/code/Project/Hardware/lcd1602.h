/********************************Copyright (c)**********************************\
**
**                   (c) Copyright 2020, Main, China, QD.
**                           All Rights Reserved
**
**                                By(wo4fisher)
**                           http://www.wo4fisher.com
**
**----------------------------------文件信息------------------------------------
** 文件名称: lcd1602.h
** 创建人员: wht
** 创建日期: 2020-03-13
** 文档描述: 
**
**----------------------------------版本信息------------------------------------
** 版本代号: V0.1
** 版本说明: 初始版本
**
**------------------------------------------------------------------------------
\********************************End of Head************************************/

#ifndef __LCD1602_H_
#define __LCD1602_H_
#include "main.h"
#include "gpio.h"

#define LCD1602_Port GPIOC
#define LCD1602_RS_Pin GPIO_PIN_5
#define LCD1602_RW_Pin GPIO_PIN_6
#define LCD1602_EN_Pin GPIO_PIN_7
#define LCD1602_D0_Pin GPIO_PIN_8
#define LCD1602_D1_Pin GPIO_PIN_9
#define LCD1602_D2_Pin GPIO_PIN_10
#define LCD1602_D3_Pin GPIO_PIN_11
#define LCD1602_D4_Pin GPIO_PIN_12
#define LCD1602_D5_Pin GPIO_PIN_13
#define LCD1602_D6_Pin GPIO_PIN_14
#define LCD1602_D7_Pin GPIO_PIN_15


/*
+++   Nima Askari
+++   www.github.com/NimaLTD
+++   www.instagram.com/github.NimaLTD
+++   Version: 1.1.0
*/

/*
0------------->x
|
|
|
|
|
\/
y
*/

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(void);
void LCD_Puts(uint8_t x, uint8_t y, char* str);//x�? y�?
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);
void LCD_CursorOn(void);
void LCD_CursorOff(void);
void LCD_ScrollLeft(void);
void LCD_ScrollRight(void);
void LCD_CreateChar(uint8_t location, uint8_t* data);
void LCD_PutCustom(uint8_t x, uint8_t y, uint8_t location);
void LCD_Put(uint8_t Data);

#endif
/********************************End of File************************************/

