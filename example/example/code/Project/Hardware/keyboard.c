#include "keyboard.h"
#include "stdio.h"


void keyboard_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
	
  HAL_GPIO_WritePin(keyboard_port, col0_pin|col1_pin|col2_pin|col3_pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = col0_pin|col1_pin|col2_pin|col3_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(keyboard_port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = row0_pin|row1_pin|row2_pin|row3_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(keyboard_port, &GPIO_InitStruct);
}

uint8_t key_scan()
{
	uint8_t key_val=0;
	uint8_t value=0xff;
	if((uint8_t)keyboard_port->IDR&0xf0)
	{
		HAL_Delay(5);
		if((uint8_t)keyboard_port->IDR&0xf0)
		{
			key_val = (uint8_t)keyboard_port->IDR&0xf0;
			keyboard_port->BSRR = 0x0e;
			keyboard_port->BSRR = (uint32_t)0x01 << 16u;
			if(!((uint8_t)keyboard_port->IDR&0xf0))
			{
				switch(key_val)
				{
					case 0x10:value=0;break;
					case 0x20:value=4;break;
					case 0x40:value=8;break;
					case 0x80:value=12;break;
				}
			}
			keyboard_port->BSRR = 0x0d;
			keyboard_port->BSRR = (uint32_t)0x02 << 16u;
			if(!((uint8_t)keyboard_port->IDR&0xf0))
			{
				switch(key_val)
				{
					case 0x10:value=1;break;
					case 0x20:value=5;break;
					case 0x40:value=9;break;
					case 0x80:value=13;break;
				}
			}
			keyboard_port->BSRR = 0x0b;
			keyboard_port->BSRR = (uint32_t)0x04 << 16u;
			if(!((uint8_t)keyboard_port->IDR&0xf0))
			{
				switch(key_val)
				{
					case 0x10:value=2;break;
					case 0x20:value=6;break;
					case 0x40:value=10;break;
					case 0x80:value=14;break;
				}
			}
			keyboard_port->BSRR = 0x07;
			keyboard_port->BSRR = (uint32_t)0x08 << 16u;
			if(!((uint8_t)keyboard_port->IDR&0xf0))
			{
				switch(key_val)
				{
					case 0x10:value=3;break;
					case 0x20:value=7;break;
					case 0x40:value=11;break;
					case 0x80:value=15;break;
				}
			}
			keyboard_port->BSRR = 0x0f;
			return value;
		}
	}
	return 16;
}

