
#include "flash.h"
#include "stm32f1xx_hal.h"
#include "dataType.h"
#include "DEFINE.h"

extern AD8402 TDS[2];  // TDS_IN:0, TDS_OUT:1
FLASH_EraseInitTypeDef EraseInit = { FLASH_TYPEERASE_PAGES, FLASH_BANK_1, ADDR_FLASH_PAGE_20, 1 };
uint32_t PageError;


void FLASH_writeParams(void)
{
	uint8_t i;
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&EraseInit, &PageError);
	
	for(i = 0; i < 20; i++) HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADDR_IN_CHANNEL_1kVALUE    + i*2,    TDS[IN_CHANNEL].arrValue_1k[i+2]);
	for(i = 0; i < 20; i++) HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADDR_IN_CHANNEL_100kVALUE  + i*2,    TDS[IN_CHANNEL].arrValue_100k[i+2]);
	for(i = 0; i < 20; i++) HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADDR_OUT_CHANNEL_1kVALUE   + i*2,    TDS[OUT_CHANNEL].arrValue_1k[i+2]);
	for(i = 0; i < 20; i++) HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADDR_OUT_CHANNEL_100kVALUE + i*2,    TDS[OUT_CHANNEL].arrValue_100k[i+2]);
	
	HAL_FLASH_Lock();
}

void FLASH_readParams(void);
