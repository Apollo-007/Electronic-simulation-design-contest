#include "lcd12864.h"
#include "stdio.h"

#define LCDLCDDisp_Off   0x3e
#define LCDLCDDisp_On    0x3f
#define Page_Add     0xb8//页地址
#define LCDCol_Add     0x40//列地址
#define Start_Line     0xC0//行地址

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


void write_commond(uint8_t com)												//对液晶写一个指令
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
void write_date(uint8_t dat)													//对液晶写一个数据
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
 void write_dc(uint8_t com,uint8_t dat)											//写一个指令和一个数据
{
	write_date(dat);
	write_commond(com);
}
//写一个指令和两个数据
void write_ddc(uint8_t com,uint8_t dat1,uint8_t dat2)
{
	write_date(dat1);
	write_date(dat2);
	write_commond(com);
}
//LCD初始化函数
void F12864_init(void)
{	
	RST_Resetpin;
//	delay_nms(2000);					
	RST_Setpin;
	write_ddc(0x40,0x00,0x00);			     						 		//设置文本显示区首地址
	write_ddc(0x41,16,0x00);  											//设置文本显示区宽度
	write_ddc(0x42,0x00,0x08);			      								//设置图形显示区首地址0x0800
	write_ddc(0x43,16,0x08);  											//设置图形显示区宽度
	write_commond(0xA0);                     							   	//设置光标形状 8x8方块
	write_commond(0x80);                     							   	//显示方式设置 文本and图形(异或)
	write_commond(0x92);                     							   	//设置光标
	write_commond(0x9F);                     							   	//显示开关设置 文本开,图形开,光标闪烁关							 	
}
void F12864_clear(void)
{
	unsigned int i;
	write_ddc(0x24,0x00,0x00); 												//置地址指针为从零开始
   	write_commond(0xb0); 													//自动写
	for(i = 0;i < 8192 ;i++)write_date(0x00); 							//清一屏
	write_commond(0xb2); 													//自动写结束
	write_ddc(0x24,0x00,0x00); 												//重置地址指针 
}
////设定显示的地址
//void goto_xy(uint8_t x,uint8_t y,uint8_t mode)
//{
//     uint32_t  temp;
//     temp = 128 / 8 * y + x;
//     if(mode)                                 								//mode = 1为Graphic
//     {                                        								//如果图形模式要加上图形区首地址0x0800
//         temp = temp + 0x0100;
//     }
//     write_ddc(0x24,temp & 0xff,temp / 256);	  							//地址指针位置
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
//		for(n = 0;n < 11;n++)                                  			  	//计数值16
//    {
//				address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
//				write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//设置显示存储器地址
//				write_dc(0xc0,*x++);                    			//只用第一个地址   （其实浪费了一般的效率up注释可删）
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
//				write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//设置显示存储器地址
//				write_dc(0xc0,GB_16[0].Mask[m++]);                    			//写入汉字字模左部
//				write_dc(0xc0,GB_16[0].Mask[m++]);                 			    //写入汉字字模右部
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
//		for(n = 0;n < 13;n++)                                  			  	//计数值16
//    	{
//		   write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//设置显示存储器地址
//		   write_dc(0xc0,GB_16[k].Mask[m++]);                    			//写入汉字字模左部
//		   write_dc(0xc0,GB_16[k].Mask[m++]);                 			    //写入汉字字模右部
//		   address = address + 128/8;                           			//修改显示存储器地址，显示下一列（共16列）
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
//		for(n = 0;n < 6;n++)                                  			  	//计数值16
//    	{
//			 address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
//		   write_ddc(0x24,(uint8_t)(address),(uint8_t)(address>>8));  			//设置显示存储器地址
//		   write_dc(0xc0,GB_0[0].Mask[m++]);                    			//写入汉字字模左部
//		   write_dc(0xc0,GB_0[0].Mask[m++]);                 			    //写入汉字字模右部
//		   X_pos += 2;                                                //下一个八位像素
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

