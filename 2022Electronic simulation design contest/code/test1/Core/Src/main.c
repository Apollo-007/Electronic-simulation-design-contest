/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "keyboard.h"
#include "stdio.h"
#include "lcd12864.h"
#include "motorstep.h"
#include "DHT11.h"
#include "at24c02.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t str[30]={0};
uint8_t key_value,key_long=16;
uint8_t key_info[] = 
{
	'1',   '2',   '3',   ' ',
	'4',   '5',   '6',   ' ',
	'7',   '8',   '9',   ' ',
	'Y',   '0',   'N',   'Q',
	16,
};

typedef struct Password
{
	uint8_t pass[6];
	uint8_t index;
	uint8_t inpass[6];
}Password;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t mytemp,myhumi,last_temp,last_humi;
uint8_t display_mode,error_num,flag;
uint32_t tick;
Password mypass = {"123789",0,"------"};

void buzzer_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
	
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void led_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
	
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void key_do()
{
	key_value = key_scan();
	if(key_value != key_long)
		key_long = key_value;
	else
		key_value = 16;
	
	if(key_value != 16)
	{
		if(display_mode == 0)
		{
			if((key_info[key_value]>='0') && (key_info[key_value]<='9') && mypass.index<6)
			{
				mypass.inpass[mypass.index++] = key_info[key_value];
				show_ascii_str(mypass.inpass,32,0);
			}
			else if(key_info[key_value] == 'Y')
			{
				if(!strncmp((const char*)mypass.inpass,(const char*)mypass.pass,6))
				{
					display_mode = 1;
					strcpy((char*)mypass.inpass,"------");
					mypass.index = 0;
					error_num = 0;
					
					
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
					HAL_Delay(500);
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
					HAL_GPIO_WritePin(MotorStep_Port, C_Pin|D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MotorStep_Port, A_Pin|B_Pin, GPIO_PIN_RESET);//开锁
					sprintf(str,"correct\r\n");
					HAL_UART_Transmit(&huart1, str, 9, 100);
					
					F12864_clear();
					sprintf(str,"T:%02dC   H:%02d%%",mytemp,myhumi);
					show_ascii_str(str,0,0);
					show_chinese_chr(0,16,0);
					show_chinese_chr(1,16,2);
					show_chinese_chr(2,16,4);//密码锁
					show_ascii_str(":001",16,6);
					
					show_chinese_chr(0,32,0);
					show_chinese_chr(1,32,2);
					show_chinese_chr(2,32,4);
					show_chinese_chr(7,32,6);
					show_chinese_chr(8,32,8);
					show_chinese_chr(9,32,10);//密码箱已打开
				}
				else
				{
					error_num++;
					strcpy((char*)mypass.inpass,"------");
					mypass.index = 0;
					
					
					if(error_num>=3)
					{
						display_mode = 2;
						
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);//蜂鸣器长响
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);//LED点亮
						sprintf(str,"error %d\r\n",error_num);
						HAL_UART_Transmit(&huart1, str, 9, 100);
						HAL_UART_Transmit(&huart1, str, 9, 100);
						HAL_UART_Transmit(&huart1, str, 9, 100);
						HAL_UART_Transmit(&huart1, str, 9, 100);
						HAL_UART_Transmit(&huart1, str, 9, 100);
						
						F12864_clear();
						sprintf(str,"T:%02dC   H:%02d%%",mytemp,myhumi);
						show_ascii_str(str,0,0);
						
						show_chinese_chr(10,16,4);
						show_chinese_chr(11,16,6);
						show_chinese_chr(12,16,8);
						show_chinese_chr(13,16,10);//错误警报
						
						show_chinese_chr(0,32,2);
						show_chinese_chr(1,32,4);
						show_chinese_chr(2,32,6);
						show_chinese_chr(7,32,8);
						show_chinese_chr(14,32,10);
						show_chinese_chr(15,32,12);//密码箱已锁死
					}
					else
					{
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
						HAL_Delay(500);
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
						sprintf(str,"error %d\r\n",error_num);
						HAL_UART_Transmit(&huart1, str, 9, 100);
						
						show_ascii_str(mypass.inpass,32,0);
						sprintf(str,"%d ",error_num);
						show_ascii_str(str,32,14);
					}
				}
			}
			else if(key_info[key_value] == 'N')
			{
				strcpy((char*)mypass.inpass,"------");
				mypass.index = 0;
				show_ascii_str(mypass.inpass,32,0);
			}
		}
		else if(display_mode == 1)
		{
			if(key_info[key_value] == 'Q')
			{
				display_mode = 0;
				HAL_GPIO_WritePin(MotorStep_Port, C_Pin|D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(MotorStep_Port, A_Pin|B_Pin, GPIO_PIN_SET);//关锁
				show_ascii_str(mypass.inpass,32,0);
				show_ascii_str("      ",32,6);
				sprintf(str,"%d ",error_num);
				show_ascii_str(str,32,14);
				show_chinese_chr(3,48,0);
				show_chinese_chr(4,48,2);//确定
				show_chinese_chr(5,48,12);
				show_chinese_chr(6,48,14);//取消
			}
		}
	}
}


void delay_ms(uint32_t ms)
{
  uint32_t ticks = ms * (SystemCoreClock / 1000); // 计算需要延时的SysTick时钟周期数
  uint32_t start = SysTick->VAL; // 获取当前的SysTick计数器值

  while (ticks > (start - SysTick->VAL)) {} // 等待SysTick计数器减到0

  return;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t buzzer_num=0;
	uint8_t data=0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim3);
	keyboard_init();	
	buzzer_init();
	led_init();
	LCD12864_Init(); 

	DHT11_Init();
	motorstep_init();
	F12864_init();
	F12864_clear();
	write_commond(0x98);
	
	sprintf(str,"T:%02dC   H:%02d%%",mytemp,myhumi);
	show_ascii_str(str,0,0);
	show_chinese_chr(0,16,0);
	show_chinese_chr(1,16,2);
	show_chinese_chr(2,16,4);//密码锁
	show_ascii_str(":001",16,6);
	show_ascii_str(mypass.inpass,32,0);
	sprintf(str,"%d ",error_num);
	show_ascii_str(str,32,14);
	show_chinese_chr(3,48,0);
	show_chinese_chr(4,48,2);//确定
	show_chinese_chr(5,48,12);
	show_chinese_chr(6,48,14);//取消
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		DHT11_Read_Data(&mytemp,&myhumi);
//		sprintf(str,"t:%d,h:%d\r\n",mytemp,myhumi);
//		HAL_UART_Transmit(&huart1,str,sizeof(str),100);
//		HAL_Delay(2000);
		
		
		if(tick%30 == 0)
		{
			DHT11_Read_Data(&mytemp,&myhumi);
			if(last_humi!=myhumi || last_temp != mytemp)
			{
				last_humi = myhumi;
				last_temp = mytemp;
				sprintf(str,"T:%02dC   H:%02d%%",mytemp,myhumi);
				show_ascii_str(str,0,0);
			}
		}
		if(tick%10 == 0)
		{
			if(mytemp>40 || myhumi>80)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
				if(buzzer_num<5)
				{
					buzzer_num++;
					sprintf(str,"abnormalTH\r\n");
					HAL_UART_Transmit(&huart1,str,sizeof(str),100);
				}
			}
			else
			{
				if(display_mode != 2)
				{
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
					buzzer_num = 0;
				}
			}
		}
		key_do();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		tick++;
//		if(tick >= 20)
//		{
//			tick = 0;
//			flag = 1;
//		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
