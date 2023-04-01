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
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd9341.h"
#include "stdio.h"
#include "mygpio.h"
#include "string.h"
#include "Stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t wave[260]={0};
uint8_t wcount=0;
float vmax=0,vmin=3000;
int vmaxc=0,vminc=0,vpp=0,freq;
bool finish_flag=0;
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void GUI_INIT()
{
		//unsigned short int i,j;

		//LCD_ShowString(100,0,100,16,16, "1KHZ");

		LCD_DrawRectangle(19,15,281,215);
	
		LCD_ShowString(1,208,18,12,12, " -2V");
		LCD_ShowString(1,170,18,12,12, " -1V");
		LCD_ShowString(1,132,18,12,12, " 0V");
		LCD_ShowString(1,94,18,12,12, " 1V");
		LCD_ShowString(1,56,18,12,12, " 2V");
		LCD_ShowString(1,18,18,12,12, " 3V");
		
		LCD_ShowString(16,218,18,12,12, "0");
		LCD_ShowString(60,218,18,12,12, "200");
		LCD_ShowString(115,218,24,12,12, "400");
		LCD_ShowString(164,218,24,12,12, "600");
		LCD_ShowString(213,218,24,12,12, "800");
		LCD_ShowString(262,218,24,12,12, "1000");
    LCD_ShowString(285,218,24,12,12, "us");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  //颜色设置
  POINT_COLOR = RED;
  BACK_COLOR = BLACK;
  int i,j,dac=0;
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	__HAL_SPI_ENABLE(&hspi1);
	GPIO_Init();
	LCD_Init();
  LCD_Display_Dir(1);               //横屏
  printf("USART TSET\n");
	Show_Str(50,51,200,16,(uint8_t*)"中文字库正常",16,0);//输出文件名	
  LCD_Clear(BLACK);
  GUI_INIT();
  HAL_TIM_Base_Start_IT(&htim3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_ADC_Start_DMA(&hadc1,wave,260);
    while(__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_TC1)==0);
    HAL_ADC_Stop_DMA(&hadc1);
    __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TC1);
    LCD_ShowString(290,20,24,12,12, "ADC");
    for(i=0;i<259;i++)
    {
      for(j=1;j<185;j++)
      {
        LCD_Fast_DrawPoint(i+20,j+25,BLACK);
      } 
      POINT_COLOR = RED;
      LCD_DrawLine(i+20,202-wave[i]*(3.3/4096)*38,i+21,202-wave[i+1]*(3.3/4096)*38);
      if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)==0)
      {
        dac=1;
      }
    }
    /*if(wave[i]>vmax)
      {
        vmax=wave[i];
        vmaxc=wcount;
      }
      if(wave[i]<vmin)
      {
        vmin=wave[i];
        vminc=wcount;
      }
      freq=(1000000.0/((vminc-vmaxc)*5));
      LCD_ShowNum(290,35,freq,5,12);
      LCD_ShowString(290,50,24,12,12,"khz");*/
    if(dac)
    {
      LCD_ShowString(290,20,24,12,12, "DAC");
      HAL_TIM_Base_Stop_IT(&htim3);
      HAL_TIM_Base_Start_IT(&htim4);
      HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,wave,260,DAC_ALIGN_12B_R);
      while(1)
      {
        while(__HAL_DMA_GET_FLAG(&hdac, DMA_FLAG_TC1)==0)
        {
          if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)!=0)
          goto here;
        }
        __HAL_DMA_CLEAR_FLAG(&hdac, DMA_FLAG_TC1);
        HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,wave,260,DAC_ALIGN_12B_R);
      }
      here:
      dac=0;
      HAL_TIM_Base_Stop_IT(&htim4);
      HAL_TIM_Base_Start_IT(&htim3);
    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
