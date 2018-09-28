
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "DEFINE.h"
#include "AD8402.h"
#include "dataProcess.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
AD8402 TDS_IN;
RX_buffer rxBuffer_t;
RX_TYPE rxTable_t, rxDebug_t;

uint8_t flag_debug_done = 0;
uint8_t flag_table_done = 0;
uint8_t flag_mode;
uint8_t flag1, flag2;
int16_t arrSkip[5];
int16_t arrCompare[25];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */



/* ham nay dieu chinh dien tro 100k sao cho gia tri ADC tra ve bam voi value
 * tra ve 1 neu gia tri dieu chinh > value
 * tra ve 0 neu gia tri dieu chinh < value
 */
uint8_t dieuChinh_100k(AD8402 *ptr, int16_t value)
{
	uint8_t *arrValue = ptr->arrValue_100k;
	int16_t *value100k = &(ptr->value_100k);
	int16_t tempV = ptr->value_100k;
	if(tempV < 0) tempV = 0;
	else if(tempV > 255) tempV = 255;
	uint8_t pos = (ptr->pos);
	int16_t temp1, temp2;
	uint8_t flag = 0;
	if(flag2 == 1)  // gia tri o ben trai value
	{
		for((*value100k) = tempV; (*value100k) > -1; (*value100k)--)
		{
			AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)(*value100k));
			DATAPROCESS_getDebugValue();
			temp1 = (*value100k);
			temp2 = rxDebug_t.value[IN_CHANNEL];
			if(rxDebug_t.value[IN_CHANNEL] > value)		// gia tri trai
			{
				temp1 = (*value100k);
				temp2 = rxDebug_t.value[IN_CHANNEL];
			}

			else		// gia tri phai
			{
				// so sanh do chenh lech gia tri trai va phai so voi value
				// neu gia tri do chenh cua gia tri trai so voi value nho hon thi lay gia tri trai
				// neu gia tri do chenh cua gia tri phai so voi value nho hon thi lay gia tri phai
				if((temp2 - value) < (value - rxDebug_t.value[IN_CHANNEL]))
				{
					arrValue[pos]  = (uint8_t)temp1;
					AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)temp1);
					flag = 1;
				}

				else
				{
					arrValue[pos] = (uint8_t)(*value100k);
					flag = 0;
				}
				break;
			}
		}
	}

	else // gia tri o ben phai value
	{
		for((*value100k) = tempV; (*value100k) < 256; (*value100k)++)
		{
			AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)(*value100k));
			DATAPROCESS_getDebugValue();
			temp1 = (*value100k);
			temp2 = rxDebug_t.value[IN_CHANNEL];
			if(rxDebug_t.value[IN_CHANNEL] < value)		// gia tri phai
			{
				temp1 = (*value100k);
				temp2 = rxDebug_t.value[IN_CHANNEL];
			}

			else		// gia tri trai
			{
				// so sanh do chenh lech gia tri trai va phai so voi value
				// neu gia tri do chenh cua gia tri trai so voi value nho hon thi lay gia tri trai
				// neu gia tri do chenh cua gia tri phai so voi value nho hon thi lay gia tri phai
				if((value - temp2) < (rxDebug_t.value[IN_CHANNEL] - value))
				{
					arrValue[pos]  = (uint8_t)temp1;
					AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)temp1);
					flag = 0;
				}

				else
				{
					arrValue[pos] = (uint8_t)(*value100k);
					flag = 1;
				}
				break;
			}
		}
	}
	return flag;
}

/* Ham nay dieu chinh dien tro 1k
 * Sau khi dieu chinh 100k, neu gia tri lay o ben trai ( > value) thi phai giam dien tro 1k
 * Neu gia tri lay o ben phai ( < value) thi phai tang dien tro 1k
 * Sau khi dieu chinh tang giam, cung so sanh gia tri do chenh lech o 2 ben 
 */
void dieuChinh_1k(AD8402 *ptr, int16_t value)
{
	uint8_t *arrValue = ptr->arrValue_1k;
	int16_t *value1k = &(ptr->value_1k);
	int16_t tempV = ptr->value_1k;
	if(tempV < 0) tempV = 0;
	else if(tempV > 255) tempV = 255;
	uint8_t pos = (ptr->pos);
	int16_t temp1, temp2;
	if(flag1 == 1) // gia tri o ben trai value
	{
		//if((*value1k) == 0) arrValue[pos] = (*value1k);			
		//else
		//{
			for((*value1k) = tempV; (*value1k) > -1; (*value1k)--)
			{
				AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)(*value1k));
				DATAPROCESS_getDebugValue();
				temp1 = (*value1k);
				temp2 = rxDebug_t.value[IN_CHANNEL];
				if(rxDebug_t.value[IN_CHANNEL] > value)		// gia tri trai
				{
					temp1 = (*value1k);
					temp2 = rxDebug_t.value[IN_CHANNEL];
				}

				else		// gia tri phai
				{
					// so sanh do chenh lech gia tri trai va phai so voi value
					// neu gia tri do chenh cua gia tri trai so voi value nho hon thi lay gia tri trai
					// neu gia tri do chenh cua gia tri phai so voi value nho hon thi lay gia tri phai
					if((temp2 - value) < (value - rxDebug_t.value[IN_CHANNEL]))
					{
						arrValue[pos]  = (uint8_t)temp1;
						AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)temp1);
						arrCompare[pos] = temp2;
					}

					else
					{
						arrValue[pos] = (uint8_t)(*value1k);
						arrCompare[pos] = rxDebug_t.value[IN_CHANNEL];
						(*value1k) = temp1;
						AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)temp1);
					}
					break;
				}
			}
		//}
	}

	else // gia tri o ben phai value
	{
		//if((*value1k) == 255) arrValue[pos] = (*value1k);
		//else
		//{
			for((*value1k) = tempV; (*value1k) < 256; (*value1k)++)
			{
				AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)(*value1k));
				for(uint8_t i = 0; i < 5; i++)
					{
						DATAPROCESS_getDebugValue();
						arrSkip[i] = rxDebug_t.value[IN_CHANNEL];
					}
				temp1 = (*value1k);
				temp2 = rxDebug_t.value[IN_CHANNEL];
				if(rxDebug_t.value[IN_CHANNEL] <  value)		// gia tri phai
				{
					temp1 = (uint8_t)(*value1k);
					temp2 = rxDebug_t.value[IN_CHANNEL];
				}

				else		// gia tri trai
				{
					// so sanh do chenh lech gia tri trai va phai so voi value
					// neu gia tri do chenh cua gia tri trai so voi value nho hon thi lay gia tri trai
					// neu gia tri do chenh cua gia tri phai so voi value nho hon thi lay gia tri phai
					if((value - temp2) < (rxDebug_t.value[IN_CHANNEL] - value))
					{
						arrValue[pos]  = (uint8_t)temp1;
						AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)temp1);
						arrCompare[pos] = temp2;
					}

					else
					{
						arrValue[pos] = (uint8_t)(*value1k);
						arrCompare[pos] = rxDebug_t.value[IN_CHANNEL];
						(*value1k) = temp1;
						AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)temp1);
					}
					break;
				}
			}	
		//}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

	TDS_IN.value_100k = TDS_IN.value_1k = 255;
	/* set gia tri max de gia tri ADC la max */
	AD8402_writeData(_100k, IN_CHANNEL, TDS_IN.value_100k);
	AD8402_writeData(_1k, IN_CHANNEL, TDS_IN.value_1k);
	
	HAL_Delay(10);
	DATAPROCESS_getAdcTable();
	DATAPROCESS_skipAckDebug();

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		DATAPROCESS_getDebugValue();
  	for(TDS_IN.pos = 2; TDS_IN.pos < rxTable_t.countVT; TDS_IN.pos++)
		{
			if(rxDebug_t.value[IN_CHANNEL] > rxTable_t.value[TDS_IN.pos]) // neu gia tri ADC gui ve lon hon gia tri co san thi moi dieu chinh
			{
				flag2 = 1;
				flag1 = dieuChinh_100k(&TDS_IN, rxTable_t.value[TDS_IN.pos]);
				dieuChinh_1k(&TDS_IN, rxTable_t.value[TDS_IN.pos]);
			}
				
			else // neu gia tri ADC gui ve lon hon gia tri co san thi moi dieu chinh
			{
				flag2 = 0;
				flag1 = dieuChinh_100k(&TDS_IN, rxTable_t.value[TDS_IN.pos]);
				dieuChinh_1k(&TDS_IN, rxTable_t.value[TDS_IN.pos]);
			}
//			else
//			{
//				flag1 = 0;
//				dieuChinh_1k(&TDS_IN, rxTable_t.value[TDS_IN.pos]);
//			}
		}

//		for(TDS_IN.value_1k = 255; TDS_IN.value_1k > - 1; TDS_IN.value_1k--)
//		{
//			AD8402_writeData(_1k, IN_CHANNEL, TDS_IN.value_1k);
//			DATAPROCESS_getDebugValue();
//		}
		while(1)
		{
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			HAL_Delay(1000);
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL15;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
