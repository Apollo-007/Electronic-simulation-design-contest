#include "step_motor.h"

uint8_t angle_data[] = {0x3,0x2,0x6,0x4,0xc,0x8,0x9,0x1};

void step_motor_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
	
  HAL_GPIO_WritePin(Step_Motor_Port, Step_Motor_A_Pin|Step_Motor_B_Pin|Step_Motor_C_Pin|Step_Motor_D_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = Step_Motor_A_Pin|Step_Motor_B_Pin|Step_Motor_C_Pin|Step_Motor_D_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Step_Motor_Port, &GPIO_InitStruct);
}

/*
只能选0，45，90，135，180，225，270，315度
*/
void step_motor_angle_set(uint16_t angle)
{
    uint8_t angle_index = angle/45;
    HAL_GPIO_WritePin(Step_Motor_Port,Step_Motor_A_Pin,(GPIO_PinState)(angle_data[angle_index]&0x1));
    HAL_GPIO_WritePin(Step_Motor_Port,Step_Motor_B_Pin,(GPIO_PinState)(angle_data[angle_index]&0x2));
    HAL_GPIO_WritePin(Step_Motor_Port,Step_Motor_C_Pin,(GPIO_PinState)(angle_data[angle_index]&0x4));
    HAL_GPIO_WritePin(Step_Motor_Port,Step_Motor_D_Pin,(GPIO_PinState)(angle_data[angle_index]&0x8));
}
