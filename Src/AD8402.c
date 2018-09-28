
#include "AD8402.h"
#include "DEFINE.h"

void AD8402_writeData(uint8_t ver, uint8_t channel, uint8_t byte)
{
	CS_1k_SET;
	CS_100k_SET;
	CLK_POT_RESET;
	
	uint8_t cnt;
	uint16_t temp;
	temp = (channel << 8) | byte;
	
	if(ver == _1k)
	{
		CS_1k_RESET;
		for(cnt = 0; cnt < 10; cnt++)
		{
			CLK_POT_RESET;
			(temp&0x0200) ? SI_POT_SET : SI_POT_RESET;		
			CLK_POT_SET;
			temp <<= 1;
		}
		CLK_POT_RESET;
		CS_1k_SET;		
	}
	
	else if(ver == _100k)
	{
		CS_100k_RESET;
		for(cnt = 0; cnt < 10; cnt++)
		{
			CLK_POT_RESET;
			(temp&0x0200) ? SI_POT_SET : SI_POT_RESET;		
			CLK_POT_SET;
			temp <<= 1;
		}
		CLK_POT_RESET;
		CS_100k_SET;
	}
}

void AD8402_write(uint8_t channel, uint8_t data_1k, uint8_t data_100k)
{
	CS_1k_SET;
	CS_100k_SET;
	CLK_POT_RESET;
	
	uint8_t cnt;
	uint16_t temp = 0, temp1 = 0, temp2 = 0;
	temp = channel << 8;
	
	/* ghi vao con 1k */
	temp1 = temp | data_1k;
	CS_1k_RESET;
	for(cnt = 0; cnt < 10; cnt++)
	{
		CLK_POT_RESET;
		(temp1&0x0200) ? SI_POT_SET : SI_POT_RESET;		
		CLK_POT_SET;
		temp1 <<= 1;
	}
	CLK_POT_RESET;
	CS_1k_SET;	
	
	/* ghi vao con 100k */
	temp2 = temp | data_100k;
	CS_100k_RESET;
	for(cnt = 0; cnt < 10; cnt++)
	{
		CLK_POT_RESET;
		(temp2&0x0200) ? SI_POT_SET : SI_POT_RESET;		
		CLK_POT_SET;
		temp2 <<= 1;
	}
	CLK_POT_RESET;
	CS_100k_SET;
}

