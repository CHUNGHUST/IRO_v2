
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
#include <stdlib.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
AD8402 TDS[2];  // TDS_IN:0, TDS_OUT:1
RX_buffer rxBuffer_t;
RX_TYPE rxAdcTable[2], rxTdsTable[2], rxDebug_t;  // IN:0, OUT:1
TEMP_VALUE temp100k_t[2], temp1k_t[2];			  // IN:0, OUT:1


uint8_t flag_debug_done = 0;
uint8_t flag_adc_done = 0;
uint8_t flag_tds_done = 0;
uint8_t flag_mode;
int16_t arrSkip[4];
int16_t arrCompare[2][25];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/*
 * Ham nay chinh tho: chinh dien tro 100k tra ve 2 gia tri o 2 ben gia tri value: vTrai > value, vPhai < value
 */

void dieuChinh_100k(AD8402 *pAD[], int16_t value[], TEMP_VALUE *pTemp100k[])
{
	uint8_t stopIn = OFF, stopOut = OFF;
	// IN_CHANNEL
	int16_t *value100k_IN = &(pAD[IN_CHANNEL]->value_100k);
	if((*value100k_IN) < DATA_MIN)  (*value100k_IN) = DATA_MIN;
	else if((*value100k_IN) > DATA_MAX)  (*value100k_IN) = DATA_MAX;
	
	int16_t *vTrai_IN = &(pTemp100k[IN_CHANNEL]->vTrai);
	int16_t *vPhai_IN = &(pTemp100k[IN_CHANNEL]->vPhai);
	int16_t *dataTrai_IN = &(pTemp100k[IN_CHANNEL]->dataTrai);
	int16_t *dataPhai_IN = &(pTemp100k[IN_CHANNEL]->dataPhai);
	// OUT_CHANNEL
	int16_t *value100k_OUT = &(pAD[IN_CHANNEL]->value_100k);
	if((*value100k_OUT) < DATA_MIN)  (*value100k_OUT) = DATA_MIN;
	else if((*value100k_OUT) > DATA_MAX)  (*value100k_OUT) = DATA_MAX;

	int16_t *vTrai_OUT = &(pTemp100k[OUT_CHANNEL]->vTrai);
	int16_t *vPhai_OUT = &(pTemp100k[OUT_CHANNEL]->vPhai);
	int16_t *dataTrai_OUT = &(pTemp100k[OUT_CHANNEL]->dataTrai);
	int16_t *dataPhai_OUT = &(pTemp100k[OUT_CHANNEL]->dataPhai);
	

	AD8402_writeData(_1k, IN_CHANNEL, DATA_MAX); // ghi gia tri max cho 1k kenh IN
	AD8402_writeData(_1k, OUT_CHANNEL, DATA_MAX); // ghi gia tri max cho 1k kenh OUT
	for((*value100k_IN) = (*value100k_IN), (*value100k_OUT) = (*value100k_OUT);;)
	{
		if((*value100k_IN) >= DATA_MIN || (*value100k_OUT) >= DATA_MIN)
		{
			if(stopIn == OFF  && (*value100k_IN) >= DATA_MIN)  AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)(*value100k_IN));
			if(stopOut == OFF && (*value100k_OUT) >= DATA_MIN) AD8402_writeData(_100k, OUT_CHANNEL, (uint8_t)(*value100k_OUT));
			HAL_Delay(100);
			DATAPROCESS_getDebugValue();

			if(stopIn == OFF && (*value100k_IN) >= DATA_MIN)
			{
				if(rxDebug_t.value[IN_CHANNEL] > value[IN_CHANNEL])    // gia tri trai
				{
					*dataTrai_IN = (*value100k_IN);				   // luu lai gia tri ben trai, luu lai du lieu ghi cho 100k ben trai value
					*vTrai_IN = rxDebug_t.value[IN_CHANNEL];  // luu gia tri ADC ben trai
					(*value100k_IN)--;
				}

				else		// gia tri phai
				{	
					*dataPhai_IN = (*value100k_IN);
					*vPhai_IN = rxDebug_t.value[IN_CHANNEL];				
					AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)(*dataTrai_IN)); // ghi lai gia tri ben trai
					stopIn = ON;
				}
			}

			if(stopOut == OFF && (*value100k_OUT) >= DATA_MIN)
			{
				if(rxDebug_t.value[OUT_CHANNEL] > value[OUT_CHANNEL])    // gia tri trai
				{
					*dataTrai_OUT = (*value100k_OUT);				   // luu lai gia tri ben trai, luu lai du lieu ghi cho 100k ben trai value
					*vTrai_OUT = rxDebug_t.value[OUT_CHANNEL];  // luu gia tri ADC ben trai
					(*value100k_OUT)--;
				}

				else		// gia tri phai
				{	
					*dataPhai_OUT = (*value100k_OUT);
					*vPhai_OUT = rxDebug_t.value[OUT_CHANNEL];				
					AD8402_writeData(_100k, OUT_CHANNEL, (uint8_t)(*dataTrai_OUT)); // ghi lai gia tri ben trai
					stopOut = ON;
				}
			}

			if(stopIn == ON && stopOut == ON) break;
		}
	}
}

void dieuChinh_1k(AD8402 *pAD[], int16_t value[],  TEMP_VALUE *pTemp100k[])
{
	uint8_t stopIn= OFF, stopOut = OFF;
	uint8_t *arrValue100k_IN = pAD[IN_CHANNEL]->arrValue_100k;  // luu byte du lieu cho 100k
	uint8_t *arrValue1k_IN   = pAD[IN_CHANNEL]->arrValue_1k;	 // luu byte du lieu cho 1k
	int16_t *value1k_IN = &(pAD[IN_CHANNEL]->value_1k);
	int16_t *value100k_IN = &(pAD[IN_CHANNEL]->value_100k);
	uint8_t pos = pAD[IN_CHANNEL]->pos;
	int16_t vPhai_IN = pTemp100k[IN_CHANNEL]->vPhai;
	int16_t dataTrai_IN = pTemp100k[IN_CHANNEL]->dataTrai;
	int16_t temp1, temp2;


	uint8_t *arrValue100k_OUT = pAD[OUT_CHANNEL]->arrValue_100k;  // luu byte du lieu cho 100k
	uint8_t *arrValue1k_OUT   = pAD[OUT_CHANNEL]->arrValue_1k;	 // luu byte du lieu cho 1k
	int16_t *value1k_OUT = &(pAD[OUT_CHANNEL]->value_1k);
	int16_t *value100k_OUT = &(pAD[OUT_CHANNEL]->value_100k);
	int16_t vPhai_OUT = pTemp100k[OUT_CHANNEL]->vPhai;
	int16_t dataTrai_OUT = pTemp100k[OUT_CHANNEL]->dataTrai;
	int16_t temp3, temp4;
	
	for(*value1k_IN = DATA_MAX, *value1k_OUT = DATA_MAX;;)
	{
		AD8402_writeData(_1k, IN_CHANNEL, (uint8_t)(*value1k_IN));
		AD8402_writeData(_1k, OUT_CHANNEL, (uint8_t)(*value1k_OUT));
		HAL_Delay(100);
		DATAPROCESS_getDebugValue();
		if((*value100k_IN) >= DATA_MIN || (*value100k_OUT) >= DATA_MIN)
		{
			if(stopIn == OFF && (*value100k_IN) >= DATA_MIN)
			{
				if((rxDebug_t.value[IN_CHANNEL] > value[IN_CHANNEL]))  // gia tri ben trai value
				{
					temp1 = rxDebug_t.value[IN_CHANNEL];
					temp2 = *value1k_IN;	
					(*value100k_IN)--;
				}

				else
				{
					if((temp1 - value[IN_CHANNEL]) < (value[IN_CHANNEL] - rxDebug_t.value[IN_CHANNEL]))
					{
						arrValue100k_IN[pos] = dataTrai_IN;
						arrValue1k_IN[pos] = temp2;
						arrCompare[IN_CHANNEL][pos] = temp1;
					}

					else
					{
						arrValue100k_IN[pos] = dataTrai_IN;
						arrValue1k_IN[pos] = *value1k_IN;
						arrCompare[IN_CHANNEL][pos] = rxDebug_t.value[IN_CHANNEL];
						stopIn = ON;
					}			
				}
			}

			if(stopOut == OFF && (*value100k_OUT) >= DATA_MIN)
			{
				if((rxDebug_t.value[OUT_CHANNEL] > value[OUT_CHANNEL]))  // gia tri ben trai value
				{
					temp3 = rxDebug_t.value[OUT_CHANNEL];
					temp4 = *value1k_OUT;	
					(*value1k_OUT)--;
				}

				else
				{
					if((temp3 - value[OUT_CHANNEL]) < (value[OUT_CHANNEL] - rxDebug_t.value[OUT_CHANNEL]))
					{
						arrValue100k_OUT[pos] = dataTrai_OUT;
						arrValue1k_OUT[pos] = temp4;
						arrCompare[OUT_CHANNEL][pos] = temp3;
					}

					else
					{
						arrValue100k_OUT[pos] = dataTrai_OUT;
						arrValue1k_OUT[pos] = *value1k_OUT;
						arrCompare[OUT_CHANNEL][pos] = rxDebug_t.value[OUT_CHANNEL];
						stopOut = ON;
					}			
				}
			}
		}

		if(stopIn == ON && stopOut == ON) break;
	}
	// Neu gia tri ben phai khi dieu chinh 100k nho hon gia tri can dieu chinh tiep theo
	// thi gia tri dieu chinh lan tiep theo phai tu gia tri ben trai
	if(vPhai_IN < rxAdcTable[IN_CHANNEL].value[pos+1])
	{
		AD8402_writeData(_100k, IN_CHANNEL, (uint8_t)dataTrai_IN); // ghi lai gia tri ben trai
		*value1k_IN = dataTrai_IN;
	}

	if(vPhai_OUT < rxAdcTable[OUT_CHANNEL].value[pos+1])
	{
		AD8402_writeData(_100k, OUT_CHANNEL, (uint8_t)dataTrai_OUT); // ghi lai gia tri ben trai
		*value1k_OUT = dataTrai_OUT;
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

	TDS[IN_CHANNEL].value_100k = DATA_MAX;
	TDS[IN_CHANNEL].value_1k = DATA_MAX;
	TDS[OUT_CHANNEL].value_100k = DATA_MAX;
	TDS[OUT_CHANNEL].value_1k = DATA_MAX;
	/* set gia tri max de gia tri ADC la max */
	AD8402_writeData(_100k, IN_CHANNEL, TDS[IN_CHANNEL].value_100k);
	AD8402_writeData(_1k, IN_CHANNEL, TDS[IN_CHANNEL].value_1k);
	AD8402_writeData(_100k, OUT_CHANNEL, TDS[OUT_CHANNEL].value_100k);
	AD8402_writeData(_1k, OUT_CHANNEL, TDS[OUT_CHANNEL].value_1k);
	
	HAL_Delay(10);
	DATAPROCESS_getAdcTable();
	DATAPROCESS_getTdsTable();
	DATAPROCESS_skipAckDebug();

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  // bo qua 2 gia tri dau de cho du lieu on dinh
		for(uint8_t i = 0; i < 2; i++)
		{
			DATAPROCESS_getDebugValue();
			arrSkip[i] = rxDebug_t.value[IN_CHANNEL];
			arrSkip[i+2] = rxDebug_t.value[OUT_CHANNEL];
		}
		
//  	for(TDS_OUT.pos = 2; TDS_OUT.pos < rxAdcTableOut_t.countVT; TDS_OUT.pos++)
//		{
//			if(rxDebug_t.value[OUT_CHANNEL] > rxAdcTableOut_t.value[TDS_OUT.pos])
//			{
//				dieuChinh_100k(&TDS_OUT, rxAdcTableOut_t.value[TDS_OUT.pos], &temp100k_t);
//				dieuChinh_1k(&TDS_OUT, rxAdcTableOut_t.value[TDS_OUT.pos], &temp100k_t);
//			}
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
