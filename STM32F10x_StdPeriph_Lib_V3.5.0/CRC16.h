//此文件16位CRC校验头文件
#include <stdio.h>
#include <stdint.h>
uint16_t usMBCRC16( uint8_t * pucFrame, uint16_t usLen );
void Append_CRC16(uint8_t * Buffer,uint16_t BufferLen);

