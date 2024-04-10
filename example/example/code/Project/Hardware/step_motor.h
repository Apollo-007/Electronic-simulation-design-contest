#ifndef __STEP_MOTOR_H
#define __STEP_MOTOR_H

#include "main.h"
#include "gpio.h"

#define Step_Motor_Port GPIOA
#define Step_Motor_A_Pin GPIO_PIN_8
#define Step_Motor_B_Pin GPIO_PIN_11
#define Step_Motor_C_Pin GPIO_PIN_12
#define Step_Motor_D_Pin GPIO_PIN_13

void step_motor_init(void);
void step_motor_angle_set(uint16_t angle);

#endif // __STEP_MOTOR_
