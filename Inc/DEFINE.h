
#ifndef _DEFINE_H_
#define _DEFINE_H_

/* giao tiep SPI voi AD8402 */
#define CLK_POT_SET      HAL_GPIO_WritePin(CLK_POT_GPIO_Port, CLK_POT_Pin, GPIO_PIN_SET)
#define CLK_POT_RESET    HAL_GPIO_WritePin(CLK_POT_GPIO_Port, CLK_POT_Pin, GPIO_PIN_RESET)

#define SI_POT_SET       HAL_GPIO_WritePin(SI_POT_GPIO_Port, SI_POT_Pin, GPIO_PIN_SET)
#define SI_POT_RESET		 HAL_GPIO_WritePin(SI_POT_GPIO_Port, SI_POT_Pin, GPIO_PIN_RESET)

#define CS_1k_SET        HAL_GPIO_WritePin(CS_1k_GPIO_Port, CS_1k_Pin, GPIO_PIN_SET)
#define CS_1k_RESET      HAL_GPIO_WritePin(CS_1k_GPIO_Port, CS_1k_Pin, GPIO_PIN_RESET)

#define CS_100k_SET      HAL_GPIO_WritePin(CS_100k_GPIO_Port, CS_100k_Pin, GPIO_PIN_SET)
#define CS_100k_RESET    HAL_GPIO_WritePin(CS_100k_GPIO_Port, CS_100k_Pin, GPIO_PIN_RESET)

// chon kenh TDS, kenh WB 
#define OUT_CHANNEL 1
#define IN_CHANNEL  0
// ver
#define _100k 1
#define _1k   0




#define ON  1
#define OFF 0

#define NO  0
#define OK  1

#define EN  1
#define DIS 0

#define DEBUG_MODE 1
#define TABLE_MODE 0

/* cac lenh giao tiep voi IRO */
#define GET_TABLE_CNO 	HAL_UART_Transmit(&huart1, (uint8_t*)"[ADC_TABLE,1]", 13, 100);
#define GET_TABLE_CNI 	HAL_UART_Transmit(&huart1, (uint8_t*)"[ADC_TABLE,0]", 13, 100);

#define DEBUG_ON				HAL_UART_Transmit(&huart1, (uint8_t*)"[DEBUG_EN,1]", 12, 100);
#define DEBUG_OFF				HAL_UART_Transmit(&huart1, (uint8_t*)"[DEBUG_EN,0]", 12, 100);

#endif
