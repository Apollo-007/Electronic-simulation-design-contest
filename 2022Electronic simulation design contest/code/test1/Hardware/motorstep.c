#include "motorstep.h"

void motorstep_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
	
  HAL_GPIO_WritePin(MotorStep_Port, C_Pin|D_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MotorStep_Port, A_Pin|B_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = A_Pin|B_Pin|C_Pin|D_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MotorStep_Port, &GPIO_InitStruct);
}


