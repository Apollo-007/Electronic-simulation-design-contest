#include "at24c02.h"

void at24c02_init()
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

//延时1μs
void mydelay_us(uint16_t n)
{
	uint16_t i = n*8;  //8MHz，周期为1/8μs
	while(i--);
}

//设置数据线模式: I-输入 O-输出
void Pin_vSDA_Mode(char status)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = SDA_Pin;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	if(status == 'I')  //输入
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
	else if(status == 'O')  //输出
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}
	HAL_GPIO_Init(at24c02_port, &GPIO_InitStruct);
}

//时钟线输出
void vSCL_Out(uint8_t dat)
{
	switch(dat)
	{
		case 0: HAL_GPIO_WritePin(at24c02_port, SCL_Pin, GPIO_PIN_RESET); break;
		default: HAL_GPIO_WritePin(at24c02_port, SCL_Pin, GPIO_PIN_SET); break;
	}
}

//写数据线
void vSDA_Out(uint8_t dat)
{
	switch(dat)
	{
		case 0: HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_RESET); break;
		default: HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET); break;
	}
}

//读数据线
uint8_t vSDA_In()
{
	GPIO_PinState PinState;
	uint8_t rt;
	PinState = HAL_GPIO_ReadPin(at24c02_port, SDA_Pin);
	switch(PinState)
	{
		case GPIO_PIN_RESET: rt = 0; break;
		default: rt = 1; break;
	}
	return rt;
}

//启动I2C通信
void I2C_Start()
{
	Pin_vSDA_Mode('O');
	vSDA_Out(1);
	mydelay_us(6);  //至少延时4.7μs
	vSCL_Out(1);
	mydelay_us(6);  //至少延时4.7μs
	vSDA_Out(0);  //下降沿
	mydelay_us(6);  //至少延时4.7μs
	vSCL_Out(0);
}

//停止I2C通信
void I2C_Stop()
{
	Pin_vSDA_Mode('O');
	vSDA_Out(0);
	mydelay_us(6);  //至少延时4.7μs
	vSCL_Out(1);
	mydelay_us(6);  //至少延时4.7μs
	vSDA_Out(1);	//上升沿
	mydelay_us(6);  //至少延时4.7μs
}

//发送应答-低电平
void I2C_Ack()
{
	Pin_vSDA_Mode('O');
	vSDA_Out(0);
	mydelay_us(6);  //至少延时4.7μs
	vSCL_Out(1);
	mydelay_us(6);  //至少延时4.7μs
	vSCL_Out(0);
	mydelay_us(6);  //至少延时4.7μs
	vSDA_Out(1);
	mydelay_us(6);  //至少延时4.7μs
}

//写1字节数据
void I2C_WtByte(uint8_t Dat)
{
	uint8_t i, tmp;
	Pin_vSDA_Mode('O');
	for(i = 0; i < 8; i++)
	{
		tmp = Dat & (0x80>>i);  //高位在前，低位在后，逐位发送
		vSCL_Out(0);
		mydelay_us(6);
		(tmp == 0) ? (vSDA_Out(0)) : (vSDA_Out(1));
		mydelay_us(6);
		vSCL_Out(1);
		mydelay_us(6);
	}
	vSCL_Out(0);
	mydelay_us(6);
	vSDA_Out(1);
	mydelay_us(6);
}

//读1字节数据
uint8_t I2C_RdByte()
{
	uint8_t Dat = 0, tmp, i;
	Pin_vSDA_Mode('I');
	vSCL_Out(0);
	mydelay_us(6);
	for(i = 0; i < 8; i++)
	{
		vSCL_Out(1);
		mydelay_us(6);
		tmp = vSDA_In();
		Dat = Dat << 1;  //读1位左移1位
		Dat = Dat | tmp;
		mydelay_us(6);
		vSCL_Out(0);
		mydelay_us(6);
	}
	return Dat;
}

//写入1字节
void AT24C02_Write(uint8_t DatAddr, uint8_t Dat)
{
	I2C_Start();		//主机发送启动通信信号
	I2C_WtByte(AT24C02_ADDR + 0);		//发送器件（芯片）地址
	I2C_Ack(); 		//产生应答响应
	I2C_WtByte(DatAddr);  //发送字地址
	I2C_Ack();		//产生应答响应
	I2C_WtByte(Dat);		//发送数据
	I2C_Ack();		//产生应答响应
	I2C_Stop();		//发送停止通信信号
}

//读取1字节
uint8_t AT24C02_Read(uint8_t DatAddr)
{
	uint8_t Dat;
	I2C_Start();		//主机发送启动通信信号
	I2C_WtByte(AT24C02_ADDR + 0);		//发送器件地址
	I2C_Ack();		//产生应答响应
	I2C_WtByte(DatAddr);		//发送字地址
	I2C_Ack();		//产生应答响应
	I2C_Start();		//再次发送启动通信信号
	I2C_WtByte(AT24C02_ADDR + 1);		//发送器件地址
	I2C_Ack();		//产生应答响应
	Dat = I2C_RdByte();		//读取数据
	I2C_Stop();		//产生非应答信号，发送停止通信信号
	return Dat;
}

