#include "lcd12864.h"
#include "stdio.h"

#define LCDLCDDisp_Off   0x3e
#define LCDLCDDisp_On    0x3f
#define Page_Add     0xb8//ҳ��ַ
#define LCDCol_Add     0x40//�е�ַ
#define Start_Line     0xC0//�е�ַ

void wait()
{
	volatile uint32_t i;
	i++;
	i++;
}

void LCD12864_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD12864_Port, DB0|DB1|DB2|DB3|DB4|DB5|DB6|DB7|WR|RD|CE|CD|RST, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = DB0|DB1|DB2|DB3|DB4|DB5|DB6|DB7|WR|RD|CE|CD|RST;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD12864_Port, &GPIO_InitStruct);
	
	F12864_init();
	F12864_clear();
	write_commond(0x98);
}

void set_db7_dir(uint8_t dir)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(LCD12864_Port, DB7, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = DB7;
	if(dir)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD12864_Port, &GPIO_InitStruct);
}


void write_commond(uint8_t com)												//��Һ��дһ��ָ��
{	 							
		uint8_t ncom = ~com;
	CE_Resetpin;	
	CD_Setpin;
	RD_Setpin;
//	HAL_GPIO_WritePin(LCD12864_Port,com,GPIO_PIN_SET);
//	HAL_GPIO_WritePin(LCD12864_Port,ncom,GPIO_PIN_RESET);
	LCD12864_Port->BSRR = (uint32_t)ncom << 16u;
	LCD12864_Port->BSRR = com;
	WR_Resetpin;																// write	  
	wait();
//	_nop_();					
	WR_Setpin;																// disable write
	CE_Setpin;						   	   		
	CD_Resetpin;
}
void write_date(uint8_t dat)													//��Һ��дһ������
{
	uint8_t ndat = ~dat;
	CE_Resetpin;
	CD_Resetpin;
	RD_Setpin;
//	HAL_GPIO_WritePin(LCD12864_Port,dat,GPIO_PIN_SET);
//	HAL_GPIO_WritePin(LCD12864_Port,ndat,GPIO_PIN_RESET);
	LCD12864_Port->BSRR = (uint32_t)ndat << 16u;
	LCD12864_Port->BSRR = dat;
	
	WR_Resetpin;
	wait();
//	_nop_();
	WR_Setpin;
	CE_Setpin;
	CD_Setpin;
}
 void write_dc(uint8_t com,uint8_t dat)											//дһ��ָ���һ������
{
	write_date(dat);
	write_commond(com);
}
//дһ��ָ�����������
void write_ddc(uint8_t com,uint8_t dat1,uint8_t dat2)
{
	write_date(dat1);
	write_date(dat2);
	write_commond(com);
}
//LCD��ʼ������
void F12864_init(void)
{	
	RST_Resetpin;
//	delay_nms(2000);					
	RST_Setpin;
	write_ddc(0x40,0x00,0x00);			     						 		//�����ı���ʾ���׵�ַ
	write_ddc(0x41,16,0x00);  											//�����ı���ʾ�����
	write_ddc(0x42,0x00,0x08);			      								//����ͼ����ʾ���׵�ַ0x0800
	write_ddc(0x43,16,0x08);  											//����ͼ����ʾ�����
	write_commond(0xA0);                     							   	//���ù����״ 8x8����
	write_commond(0x80);                     							   	//��ʾ��ʽ���� �ı�andͼ��(���)
	write_commond(0x92);                     							   	//���ù��
	write_commond(0x9F);                     							   	//��ʾ�������� �ı���,ͼ�ο�,�����˸��							 	
}
void F12864_clear(void)
{
	unsigned int i;
	write_ddc(0x24,0x00,0x00); 												//�õ�ַָ��Ϊ���㿪ʼ
   	write_commond(0xb0); 													//�Զ�д
	for(i = 0;i < 8192 ;i++)write_date(0x00); 							//��һ��
	write_commond(0xb2); 													//�Զ�д����
	write_ddc(0x24,0x00,0x00); 												//���õ�ַָ�� 
}
////�趨��ʾ�ĵ�ַ
//void goto_xy(uint8_t x,uint8_t y,uint8_t mode)
//{
//     uint32_t  temp;
//     temp = 128 / 8 * y + x;
//     if(mode)                                 								//mode = 1ΪGraphic
//     {                                        								//���ͼ��ģʽҪ����ͼ�����׵�ַ0x0800
//         temp = temp + 0x0100;
//     }
//     write_ddc(0x24,temp & 0xff,temp / 256);	  							//��ַָ��λ��
//}
//void dprintf_hanzi_string_1(/*struct typFNT_GB16 *GB_16*/ unsigned char * x,uint32_t X_pos,uint32_t Y_pos,uint8_t j,uint8_t k)
//{  
//	unsigned int address;
//	unsigned char n;
//	long int m = 0;
//	
//	while(k < j)
//	{
//		
//		for(n = 0;n < 11;n++)                                  			  	//����ֵ16
//    {
//				address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
//				write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//������ʾ�洢����ַ
//				write_dc(0xc0,*x++);                    			//ֻ�õ�һ����ַ   ����ʵ�˷���һ���Ч��upע�Ϳ�ɾ��
//				X_pos += 1;
//		}
//			X_pos += 5;
//			k++;
//	}
//}           
//void dprintf_hanzi_string_2(struct typFNT_GB16 *GB_16,uint32_t X_pos,uint32_t Y_pos,uint8_t j,uint8_t k)
//{  
//	unsigned int address;
//	long int m = 0;
//	address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
//	while(k < j)
//	{
//				write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//������ʾ�洢����ַ
//				write_dc(0xc0,GB_16[0].Mask[m++]);                    			//д�뺺����ģ��
//				write_dc(0xc0,GB_16[0].Mask[m++]);                 			    //д�뺺����ģ�Ҳ�
//			address = address + 128/8;
//			k++;
//		}
//}
//void dprintf_hanzi_string_3(struct typFNT_GB16 *GB_16,uint32_t X_pos,uint32_t Y_pos,uint8_t j,uint8_t k)
//{  
//	unsigned int address;
//	unsigned char m,n;
//	while(k < j)
//	{
//		m = 0;
//		address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
//		for(n = 0;n < 13;n++)                                  			  	//����ֵ16
//    	{
//		   write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//������ʾ�洢����ַ
//		   write_dc(0xc0,GB_16[k].Mask[m++]);                    			//д�뺺����ģ��
//		   write_dc(0xc0,GB_16[k].Mask[m++]);                 			    //д�뺺����ģ�Ҳ�
//		   address = address + 128/8;                           			//�޸���ʾ�洢����ַ����ʾ��һ�У���16�У�
//	    }
//		X_pos += 2;
//		k++;
//	}
//}
//void dprintf_hanzi_string_4(struct typFNT_GB0 *GB_0,uint32_t X_pos,uint32_t Y_pos,uint8_t j,uint8_t k)
//{  
//	unsigned int address;
//	unsigned char m,n;
//	m = 0;
//	while(k < j)
//	{
//		
//		for(n = 0;n < 6;n++)                                  			  	//����ֵ16
//    	{
//			 address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
//		   write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//������ʾ�洢����ַ
//		   write_dc(0xc0,GB_0[0].Mask[m++]);                    			//д�뺺����ģ��
//		   write_dc(0xc0,GB_0[0].Mask[m++]);                 			    //д�뺺����ģ�Ҳ�
//		   X_pos += 2;                                                //��һ����λ����
//	    }
//		X_pos += 4;
//		k++;
//	}
//}

void set_addr(uint8_t x,uint8_t y)
{
	unsigned int address;
	address = (LcmLengthDots>>3)*x + y + 0x0800;
	write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));
}

void draw_char(uint8_t chr,uint8_t x,uint8_t y)
{
	uint8_t line;
	unsigned int address;
	uint16_t point = (chr-' ')<<4;
	for(line=0;line<16;line++)
	{
//		set_addr(x,y);
		
	address = (LcmLengthDots>>3)*x + y + 0x0800;
	write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));
		write_dc(0xc0,ASCII[point+line]);
		x++;
	}
}

void show_ascii_str(uint8_t *buf,uint8_t x,uint8_t y)
{
	uint8_t j=0;
	while(buf[j]!='\0')
	{
		draw_char(buf[j],x,y);
		y++;
		j++;
	}
}

void show_chinese_chr(uint8_t buf,uint8_t x,uint8_t y)
{
	uint8_t gezi,line;
	unsigned int address;
	uint16_t point = buf*32;
	for(line=0;line<16;line++)
	{
		for(gezi=0;gezi<2;gezi++)
		{
//			set_addr(x+line,y+gezi);
			address = (LcmLengthDots>>3)*(x+line) + (y+gezi) + 0x0800;
	write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));
			write_dc(0xc0,Chi[point+line*2+gezi]);
		}
	}
}

