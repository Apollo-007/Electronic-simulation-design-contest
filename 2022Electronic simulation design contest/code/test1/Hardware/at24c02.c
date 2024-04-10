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

//��ʱ1��s
void mydelay_us(uint16_t n)
{
	uint16_t i = n*8;  //8MHz������Ϊ1/8��s
	while(i--);
}

//����������ģʽ: I-���� O-���
void Pin_vSDA_Mode(char status)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = SDA_Pin;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	if(status == 'I')  //����
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
	else if(status == 'O')  //���
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}
	HAL_GPIO_Init(at24c02_port, &GPIO_InitStruct);
}

//ʱ�������
void vSCL_Out(uint8_t dat)
{
	switch(dat)
	{
		case 0: HAL_GPIO_WritePin(at24c02_port, SCL_Pin, GPIO_PIN_RESET); break;
		default: HAL_GPIO_WritePin(at24c02_port, SCL_Pin, GPIO_PIN_SET); break;
	}
}

//д������
void vSDA_Out(uint8_t dat)
{
	switch(dat)
	{
		case 0: HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_RESET); break;
		default: HAL_GPIO_WritePin(at24c02_port, SDA_Pin, GPIO_PIN_SET); break;
	}
}

//��������
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

//����I2Cͨ��
void I2C_Start()
{
	Pin_vSDA_Mode('O');
	vSDA_Out(1);
	mydelay_us(6);  //������ʱ4.7��s
	vSCL_Out(1);
	mydelay_us(6);  //������ʱ4.7��s
	vSDA_Out(0);  //�½���
	mydelay_us(6);  //������ʱ4.7��s
	vSCL_Out(0);
}

//ֹͣI2Cͨ��
void I2C_Stop()
{
	Pin_vSDA_Mode('O');
	vSDA_Out(0);
	mydelay_us(6);  //������ʱ4.7��s
	vSCL_Out(1);
	mydelay_us(6);  //������ʱ4.7��s
	vSDA_Out(1);	//������
	mydelay_us(6);  //������ʱ4.7��s
}

//����Ӧ��-�͵�ƽ
void I2C_Ack()
{
	Pin_vSDA_Mode('O');
	vSDA_Out(0);
	mydelay_us(6);  //������ʱ4.7��s
	vSCL_Out(1);
	mydelay_us(6);  //������ʱ4.7��s
	vSCL_Out(0);
	mydelay_us(6);  //������ʱ4.7��s
	vSDA_Out(1);
	mydelay_us(6);  //������ʱ4.7��s
}

//д1�ֽ�����
void I2C_WtByte(uint8_t Dat)
{
	uint8_t i, tmp;
	Pin_vSDA_Mode('O');
	for(i = 0; i < 8; i++)
	{
		tmp = Dat & (0x80>>i);  //��λ��ǰ����λ�ں���λ����
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

//��1�ֽ�����
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
		Dat = Dat << 1;  //��1λ����1λ
		Dat = Dat | tmp;
		mydelay_us(6);
		vSCL_Out(0);
		mydelay_us(6);
	}
	return Dat;
}

//д��1�ֽ�
void AT24C02_Write(uint8_t DatAddr, uint8_t Dat)
{
	I2C_Start();		//������������ͨ���ź�
	I2C_WtByte(AT24C02_ADDR + 0);		//����������оƬ����ַ
	I2C_Ack(); 		//����Ӧ����Ӧ
	I2C_WtByte(DatAddr);  //�����ֵ�ַ
	I2C_Ack();		//����Ӧ����Ӧ
	I2C_WtByte(Dat);		//��������
	I2C_Ack();		//����Ӧ����Ӧ
	I2C_Stop();		//����ֹͣͨ���ź�
}

//��ȡ1�ֽ�
uint8_t AT24C02_Read(uint8_t DatAddr)
{
	uint8_t Dat;
	I2C_Start();		//������������ͨ���ź�
	I2C_WtByte(AT24C02_ADDR + 0);		//����������ַ
	I2C_Ack();		//����Ӧ����Ӧ
	I2C_WtByte(DatAddr);		//�����ֵ�ַ
	I2C_Ack();		//����Ӧ����Ӧ
	I2C_Start();		//�ٴη�������ͨ���ź�
	I2C_WtByte(AT24C02_ADDR + 1);		//����������ַ
	I2C_Ack();		//����Ӧ����Ӧ
	Dat = I2C_RdByte();		//��ȡ����
	I2C_Stop();		//������Ӧ���źţ�����ֹͣͨ���ź�
	return Dat;
}

