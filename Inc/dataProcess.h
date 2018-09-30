
#ifndef _DATAPROCESS_H_
#define _DATAPROCESS_H_

#include "stm32f1xx_hal.h"
#include "dataType.h"



/*
 * @biref: tach dau '['  ','  ']'
 * @param: *RX: con tro tro den cau truc nhan ( cua TABLE, cua DEBUG)
 * @param: *rxBuf : con tro tro den cau truc du lieu dem nhan ve qua UART
 * @retval: khong
 * @note: vi tri cac dau luu vao RX->arrLVT 
 *        vi tri phan tu cuoi cung cua arrLVT luu vao RX->countVT
 *        gia tri sau khi tach ra duoc luu vao RX->value
 * VI DU: ban tin nhan ve: [ADC_TABLE,0,123,456,789]
 * =>arrLVT = {0,10,12,16,20,23}, 0 23 laf vi tri cua '[' ']'
 * =>countVT = 5
 * =>value: chua dung
 */
static void DATAPROCESS_tachDau(RX_TYPE *RX, RX_buffer *rxBuf);

/*
 * @biref: tach chuoi tu chuoi nguon, luu vao chuoi dich
 * @param: *des: con tro tro den mang dich
 * @param: *src: con tro tro den mang dau vao
 * @param: begin: cat chuoi tu vi tri begin
 * @param: length: do dai muon cat
 * @retval: khong
 */
static void DATAPROCESS_tachChuoi(uint8_t *des, uint8_t *src, uint8_t begin, uint8_t length);

/*
 * @biref: tach gia tri ra o giua cac cap dau '['  ','  ']'
 * @param: *RX: con tro tro den cau truc nhan ( cua TABLE, cua DEBUG)
 * @param: *rxBuf : con tro tro den cau truc du lieu dem nhan ve qua UART
 * @retval: khong
 * @note: phai goi ham tach dau truoc khi goi ham nay,  gia tri sau khi tach ra duoc luu vao RX->value
 * VI DU: ban tin nhan ve: [ADC_TABLE,0,123,456,789] sau khi tach dau duoc arrLVT = {0,10,12,16,20,23}, countVT = 5
 * =>value = {0,0,123,456,789}
 */
static void DATAPROCESS_tachGiatri(RX_TYPE *RX, RX_buffer *rxBuf);

/*
 * @biref: lay gia tri kieu int16_t luu vao RX->value
 * @param: *flag: co bao nhan xong chuoi du lieu TABLE hay DEBUG
 * @param: *RX: rxTable_t or rxDebug_t
 * @retval: khong
 */
void DATAPROCESS_layGiaTri(uint8_t *flag, RX_TYPE *RX);

/* 
 * @biref: lay bang ADC_TABLE
 */
void DATAPROCESS_getAdcTable(void);

/* 
 * @biref: lay bang TDS_TABLE
 */
void DATAPROCESS_getTdsTable(void);

/* 
 * @biref: bo qua chuoi ban tin xac nhan DEBUG
 */
void DATAPROCESS_skipAckDebug(void);

/* 
 * @biref: lay bang DEBUG
 */
void DATAPROCESS_getDebugValue(void);

#endif
