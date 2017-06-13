/**********this is my function library**********************/
#include "stdint.h"
#define CLI()      __set_PRIMASK(1)		/* 关闭总中断 */  
#define SEI() __set_PRIMASK(0)				/* 开放总中断 */ 

#define minus 0
#define postlive 1
void Usart1_Config(uint32_t Baudrate);
void Usart2_Config(uint32_t BaudRate);
void USART1_IRQHandler(void);
void Flash_Write(uint32_t address,uint32_t Data);
uint16_t Flash_Read(uint32_t address);
void Flash_Clear(uint32_t address);
void Tpt_and_Hmp_Count(uint8_t HByte,uint8_t LByte,uint8_t type);

