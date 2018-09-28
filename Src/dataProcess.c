
#include "dataProcess.h"
#include "dataType.h"
#include "DEFINE.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

extern RX_buffer rxBuffer_t;
extern RX_TYPE rxTable_t, rxDebug_t;
extern uint8_t flag_debug_done;
extern uint8_t flag_table_done;
extern uint8_t flag_mode;

extern UART_HandleTypeDef huart1;


static void DATAPROCESS_tachDau(RX_TYPE *RX, RX_buffer *rxBuf)
{
	uint8_t *ptrLVT  = RX->arrLVT;
	uint8_t cnt;
	uint8_t countVT = 0;
	
	uint8_t *ptrBufferRX = rxBuf->arrRX;
	int8_t countRX = rxBuf->countRX;

	ptrLVT[countVT] = 0;  /* vi tri dau '[' */
	for(cnt = 1; cnt < countRX; cnt++)
	{
		if(ptrBufferRX[cnt] == ',')
		{
			countVT++;
			ptrLVT[countVT] = cnt;
		}
	}
	countVT++;
	RX->countVT = countVT;
	ptrLVT[countVT] = countRX; /* vi tri dau ']' */
	ptrLVT[countVT+1] = 0xFF; /* thong bao ket thuc mang LVT */
}

/* destination : dich den */
static void DATAPROCESS_tachChuoi(uint8_t *des, uint8_t *src, uint8_t begin, uint8_t length)
{
	memcpy(des, (const char*)(src+begin), length);
	des[length] = '\0';
}

/* tach tu mang rx buffer cac gia tri luu vao value */
static void DATAPROCESS_tachGiatri(RX_TYPE *RX, RX_buffer *rxBuf)
{
	
	DATAPROCESS_tachDau(RX, rxBuf);
	
	uint8_t *ptrLVT = RX->arrLVT;
	int8_t countVT = RX->countVT;
	int16_t *ptrValue = RX->value;
	
	uint8_t *ptrBufferRX = rxBuf->arrRX;	
	uint8_t buf[15];

	for(uint8_t cnt = 0; cnt < countVT; cnt++)
	{
		DATAPROCESS_tachChuoi(buf, ptrBufferRX, ptrLVT[cnt]+1, ptrLVT[cnt+1] - ptrLVT[cnt] - 1);
		ptrValue[cnt] = atoi((const char*)buf);
	}
}

void DATAPROCESS_layGiaTri(uint8_t *flag, RX_TYPE *RX)
{
	rxBuffer_t.arrRX[0] = '\0'; // xoa ky tu dau di de kiem tra xem co nhan dung chuoi ban tin dang [xzxcadasd] hay khong
	//HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxBuffer_t.byteRX, 1);				
	while(*flag == NO) // cho den khi nhan duoc chuoi ban tin moi [asdadsa], trong khi chua day du ban tin thi nhay led 20ms
	{
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_Delay(20);
	}
	*flag = NO;							
	DATAPROCESS_tachGiatri(RX, &rxBuffer_t);  // nhan duoc 1 chuoi ban tin thi dem di xu ly, tach ra gia tri luu vao mang rxDebug_t.value
}

void DATAPROCESS_getAdcTable()
{
	/* cho phep nhan 1 ky tu roi nhay vao ngat */
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxBuffer_t.byteRX, 1);
	flag_mode = TABLE_MODE;
	GET_TABLE_CNI;
	
	DATAPROCESS_layGiaTri(&flag_table_done, &rxTable_t);
}

void DATAPROCESS_skipAckDebug()
{
	// BAT DAU LAY DU LIEU DEBUG O DAY, BO QUA DU LIEU XAC NHAN TRA VE lAN DAU
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxBuffer_t.byteRX, 1);
	flag_mode = DEBUG_MODE;
	DEBUG_ON;        									 /* gui lenh nhan gia tri debug */
	while(flag_debug_done == NO)
  {
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_Delay(100);
	}  // bo qua chuoi trang thai debug tra ve [DEBUG_EN,0]
	flag_debug_done = NO;	
	// KET THUC NHAN DU LIEU XAC NHAN 
}

void DATAPROCESS_getDebugValue()
{
	/* cho phep nhan 1 ky tu roi nhay vao ngat */
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxBuffer_t.byteRX, 1);
		
//	flag_mode = DEBUG_MODE;
//	DEBUG_ON; 
	DATAPROCESS_layGiaTri(&flag_debug_done, &rxDebug_t);
}
