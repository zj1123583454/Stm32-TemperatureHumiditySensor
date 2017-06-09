/**********this is my function library**********************/
#include "stdint.h"
#define CLI()      __set_PRIMASK(1)		/* �ر����ж� */  
#define SEI() __set_PRIMASK(0)				/* �������ж� */ 
void Usart1_Config(uint32_t Baudrate);
void Usart2_Config(uint32_t BaudRate);
void USART1_IRQHandler(void);
void Flash_Write(uint32_t address,uint32_t Data);
uint16_t Flash_Read(uint32_t address);
void Flash_Clear(uint32_t address);
