
#ifndef _AD8402_H_
#define _AD8402_H_

#include "stm32f1xx_hal.h"
void AD8402_writeData(uint8_t ver, uint8_t channel, uint8_t byte);
void AD8402_write(uint8_t channel, uint8_t data_1k, uint8_t data_100k);
#endif
