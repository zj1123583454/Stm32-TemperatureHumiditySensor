#include "function.h"
#include "main.h"
#define USART1_TI            //如果不使用接受中断模式 注释此行
extern uint8_t Tpt_Hmp_ValueHByte;
extern uint8_t Tpt_Hmp_ValueLByte;
uint8_t Usart1_RxBuffer[10]={0};
uint8_t Number=0;
USART_InitTypeDef	USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
//USART1功能及相应GPIO配置
void Usart1_Config(uint32_t Baudrate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	USART_DeInit(USART1);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//Usart-RX_Pin_Config
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//Usart-Function_Config
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  USART_InitStructure.USART_BaudRate =Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
#ifdef USART1_TI	
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
#endif
	USART_Cmd(USART1,ENABLE);
}
void Usart2_Config(uint32_t BaudRate)
{
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	USART_DeInit(USART2);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_InitStructure.USART_BaudRate=BaudRate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2,ENABLE);
}
void LED_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
//USART1接收中断函数
#ifdef USART1_TI
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		if(Number<8)
			Usart1_RxBuffer[Number++]=USART_ReceiveData(USART1);
	}
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
}
#endif
void Flash_Write(uint32_t address,uint32_t Data)
{
		CLI();								// 暂时关闭总中断  因为Flash的擦除过程中会受中断事件影响
		FLASH_Unlock();   
		FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);   
		FLASH_ErasePage(address);   
		FLASH_ProgramHalfWord((address),Data%256);  //flash 一个单位地址存储256		所以此处写入低八位
		FLASH_ProgramHalfWord((address+4),Data/256);  //flash 一个单位地址存储256		所以此处写入高八位

		FLASH_Lock();
		SEI();								// 重新打开总中断
}

uint16_t Flash_Read(uint32_t address)
{
		uint32_t Val;
	  Val = *(uint32_t *)(address);
		return Val;
}
void Flash_Clear(uint32_t address)
{
		CLI();								// 暂时关闭总中断  因为Flash的擦除过程中会受中断事件影响
		FLASH_Unlock();   
		FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);   
		FLASH_ErasePage(address);
		FLASH_Lock();
		SEI();		
}
void Tpt_and_Hmp_Count(uint8_t HByte,uint8_t LByte,uint8_t type)
{
	uint16_t CRCValue1=0x00;
	uint16_t CRCVlaue2=0x00;
	Tpt_Hmp_ValueHByte=0;
	Tpt_Hmp_ValueLByte=0;
	CRCValue1=((HByte * 256 + LByte)/10);
	if(type==minus)
		CRCValue1= -CRCValue1;
	
	Tpt_Hmp_ValueLByte=(CRCVlaue2=(CRCValue1<<8))>>8;
	Tpt_Hmp_ValueHByte=CRCValue1>>8;
}
