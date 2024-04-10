#ifndef __MOTORSTEP_H
#define __MOTORSTEP_H

#include "main.h"
#include "gpio.h"

#define MotorStep_Port GPIOA
#define A_Pin GPIO_PIN_8
#define B_Pin GPIO_PIN_11
#define C_Pin GPIO_PIN_12
#define D_Pin GPIO_PIN_15

#define A_Set(val) HAL_GPIO_WritePin(MotorStep_Port, A_Pin, (GPIO_PinState)val)
#define B_Set(val) HAL_GPIO_WritePin(MotorStep_Port, B_Pin, (GPIO_PinState)val)
#define C_Set(val) HAL_GPIO_WritePin(MotorStep_Port, C_Pin, (GPIO_PinState)val)
#define D_Set(val) HAL_GPIO_WritePin(MotorStep_Port, D_Pin, (GPIO_PinState)val)

void motorstep_init(void);

#endif
