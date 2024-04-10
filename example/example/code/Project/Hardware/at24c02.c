#include "at24c02.h"

#define FAILURE 0
#define SUCCESS 1

//配置SDA信号线为输入模式
void SDA_Input_Mode()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = SDA_Pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(at24c02_port, &GPIO_InitStruct);
}

//配置SDA信号线为输出模式
void SDA_Output_Mode()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = SDA_Pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(at24c02_port, &GPIO_InitStruct);
}

//
void SDA_Output( uint16_t val )
{
	if ( val ) {
		HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_RESET);
	}
}

//
void SCL_Output( uint16_t val )
{
	if ( val ) {
		HAL_GPIO_WritePin(at24c02_port, SCL_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(at24c02_port, SCL_Pin, GPIO_PIN_RESET);
	}
}

//
uint8_t SDA_Input()
{
	return HAL_GPIO_ReadPin(at24c02_port,SDA_Pin);
}

//延时程序
void delay1(unsigned int n)
{
	unsigned int i;
	for ( i=0;i<n;++i);
}

//I2C总线启动
void I2CStart(void)
{
	SDA_Output(1);delay1(500);
	SCL_Output(1);delay1(500);
	SDA_Output(0);delay1(500);
	SCL_Output(0);delay1(500);
}

//I2C总线停止
void I2CStop(void)
{
	SCL_Output(0); delay1(500);
	SDA_Output(0); delay1(500);
	SCL_Output(1); delay1(500);
	SDA_Output(1); delay1(500);

}

//等待应答
unsigned char I2CWaitAck(void)
{
	unsigned short cErrTime = 5;
	SDA_Input_Mode(); 
	delay1(500);
	SCL_Output(1);delay1(500);
	while(SDA_Input())
	{
		cErrTime--;
		delay1(500);
		if (0 == cErrTime)
		{
			SDA_Output_Mode();
			I2CStop();
			return FAILURE;
		}
	}
	SDA_Output_Mode();
	SCL_Output(0);delay1(500); 
	return SUCCESS;
}

//发送应答位
void I2CSendAck(void)
{
	SDA_Output(0);delay1(500);
	delay1(500);
	SCL_Output(1); delay1(500);
	SCL_Output(0); delay1(500);

}

//
void I2CSendNotAck(void)
{
	SDA_Output(1);
	delay1(500);
	SCL_Output(1); delay1(500);
	SCL_Output(0); delay1(500);

}

//通过I2C总线发送一个字节数据
void I2CSendByte(unsigned char cSendByte)
{
	unsigned char  i = 8;
	while (i--)
	{
		SCL_Output(0);delay1(500); 
		SDA_Output(cSendByte & 0x80); delay1(500);
		cSendByte += cSendByte;
		delay1(500); 
		SCL_Output(1);delay1(500); 
	}
	SCL_Output(0);delay1(500); 
}

//从I2C总线接收一个字节数据
unsigned char I2CReceiveByte(void)
{
	unsigned char i = 8;
	unsigned char cR_Byte = 0;
	SDA_Input_Mode(); 
	while (i--)
	{
		cR_Byte += cR_Byte;
		SCL_Output(0);delay1(500); 
		delay1(500); 
		SCL_Output(1);delay1(500); 
		cR_Byte |=  SDA_Input(); 
	}
	SCL_Output(0);delay1(500); 
	SDA_Output_Mode();
	return cR_Byte;
}

//I2C总线初始化
void AT24C02_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(at24c02_port, SCL_Pin|SDA_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PAPin PAPin */
	GPIO_InitStruct.Pin = SCL_Pin|SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(at24c02_port, &GPIO_InitStruct);

}

// AT24C02 读
void AT24C02_Read(unsigned char* pucBuf, unsigned char ucAddr, unsigned char ucNum)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(ucAddr);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	while(ucNum--)
	{
	*pucBuf++ = I2CReceiveByte();
	if(ucNum)
	I2CSendAck();
	else
	I2CSendNotAck();
	}
	I2CStop();
}

// AT24C02 写
void AT24C02_Write(unsigned char* pucBuf, unsigned char ucAddr, unsigned char ucNum)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(ucAddr);
	I2CWaitAck();
	while(ucNum--)
	{
		I2CSendByte(*pucBuf++);
		I2CWaitAck();
	}
	I2CStop();
	delay1(500);
}
