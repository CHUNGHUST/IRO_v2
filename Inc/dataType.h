
#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include "stm32f1xx_hal.h"

/* cau truc luu du lieu cua dien tro */
typedef struct
{
	uint8_t arrValue_1k[25];   // mang luu gia tri byte du lieu cua bien tro 1k
	uint8_t arrValue_100k[25]; // mang luu gia tri byte du lieu cua bien tro 100k
	int16_t value_1k;    	   // gia tri dien tro cua bien tro 1k
	int16_t value_100k;  	   // gia tri dien tro cua bien tro 100k
	uint8_t pos; 			   // bien luu vi tri dien tro
} AD8402;

/* cau truc cua ban tin nhan ve luu vao buffer */
typedef struct
{
	uint8_t byteRX;     /* byte nhan ve sau 1 lan ngat */
	uint8_t arrRX[110]; /* mang du lieu nhan duoc sau khi ket thuc chuoi */
	int8_t countRX;     /* bien dem so luong byte nhan duoc */
} RX_buffer;


/* cau truc ban tin TABLE, DEBUG ..*/
typedef struct
{
	uint8_t arrLVT[30]; /* mang luu vi tri cac dau '['   ','   ']' */
	uint8_t countVT;    /* bien dem so luong phan tu cuar arrLVT */	
	int16_t value[25];  /* mang luu gia tri khi tach ra */
} RX_TYPE;

typedef struct
{
	int16_t vADC_Trai;		// gia tri ADC ben trai value khi dieu chinh 100k
	int16_t vADC_Phai;		// gia tri ADC ben phai value khi dieu chinh 100k
	int16_t dataTrai;		// gia tri du lieu ben trai value ghi cho 100k
	int16_t dataPhai;		// gia tri du lieu ben phai value ghi cho 100k
} TEMP_VALUE;

#endif
