/* Includes ------------------------------------------------------------------*/
#include "main.h"
#ifdef __GNUC__

  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

extern uint8_t Usart1_RxBuffer[10];
extern uint8_t Number;
uint32_t Flash_Address=0x800800a;
uint8_t Tpt_Hmp_ValueHByte=0;
uint8_t Tpt_Hmp_ValueLByte=0;	
int main(void)
{
	uint8_t i=0;																 //循环变量
	uint8_t Temperature_HByre=0;                 //温度高位
	uint8_t Temperature_LByre=0;								 //温度地位
	uint8_t Humidity_HByte=0;										 //湿度高位
	uint8_t Humidity_LByte=0;                    //湿度地位
	uint8_t Device_Address;
	uint8_t Usart2_RxBuffer[10]={0};                                      	//Usart2接收数据缓冲区
	uint8_t Usart1_Command1[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0a};    //温度查询命令(默认针对01查询)
	uint8_t Usart1_Command2[8]={0x01,0x03,0x00,0x01,0x00,0x01,0xd5,0xca};		//湿度查询命令(默认针对01查询)
	uint8_t Usart1_Command3[8]={0x01,0x03,0x00,0x00,0x00,0x02,0xc4,0x0b}; 		//温湿度查询命令 (默认针对01查询)
	uint8_t Usart1_Command4[8]={0xff,0x03,0x00,0x02,0x00,0x01,0x30,0x14};    //查询地址命令(此命令固定不变)
	uint8_t Usart1_Command5[8]={0xff,0x06,0x00,0x02,0x00};                        
	
	uint8_t retvalue1[10]={0x01,0x03,0x02};      //温度返回数据(默认针对01返回)
	uint8_t retvalue2[10]={0x01,0x03,0x02};      //湿度返回数据(默认针对01返回)
	uint8_t retvalue3[10]={0x01,0x03,0x04};     //温湿度返回数据(默认针对01返回)
	uint8_t retvalue4[10]={0xff,0x03,0x02,0x00,0x01};     								//查询地址返回数据
	
	uint32_t Time1,Time2;
	Usart1_Config(9600);												 //开启Usart1(接收中断模式)
	Usart2_Config(115200);											 //开启Usart2(输入输出模式)
	for(Time1=1000;Time1>0;Time1--)
	{
		for(Time2=1000;Time2>0;Time2--);
	}
	USART_SendData(USART2,0xaa);                 //使能温湿度传感器的串口输出模式
	Device_Address=Flash_Read(Flash_Address);
	if(Device_Address==0xff || Device_Address==0x00)
	{
		Flash_Clear(Flash_Address);
		Flash_Write(Flash_Address,0x01);           //默认设备地址为01
		Device_Address=Flash_Read(Flash_Address);
	}

	//刷新设备号 并且计算CRC校验追加到数组末尾生成完整指令
	Usart1_Command1[0]=Device_Address;          
	Append_CRC16(Usart1_Command1,6);
	Usart1_Command2[0]=Device_Address;
	Append_CRC16(Usart1_Command1,6);
	Usart1_Command3[0]=Device_Address;
	Append_CRC16(Usart1_Command3,6);

  while (1)
  {
		if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE))
		{
			if(i<8)
				Usart2_RxBuffer[i++]=USART_ReceiveData(USART2);
			else 
				i=0;
		}
		if(Usart2_RxBuffer[0]==0xaa && Usart2_RxBuffer[7]==0xff)
		{
			Temperature_HByre=0;
			Temperature_LByre=0;
			Humidity_HByte=0;
			Humidity_LByte=0;
			Tpt_and_Hmp_Count(Usart2_RxBuffer[2],Usart2_RxBuffer[3]);
			Temperature_HByre=Tpt_Hmp_ValueHByte;
			Temperature_LByre=Tpt_Hmp_ValueLByte;
			Tpt_and_Hmp_Count(Usart2_RxBuffer[4],Usart2_RxBuffer[5]);
			Humidity_HByte=Tpt_Hmp_ValueHByte;
			Humidity_LByte=Tpt_Hmp_ValueLByte;
		}
		if(Number==8)
		{
					if(memcmp(Usart1_Command1,Usart1_RxBuffer,8)==0) //查询温度
					{
						retvalue1[0]=Device_Address;	
						retvalue1[3]=Temperature_HByre;
						retvalue1[3]=Temperature_LByre;
						Append_CRC16(retvalue1,5);
						for(i=0;i<7;i++)
						{
							USART_SendData(USART1,retvalue1[i]);
							while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
						}
					}
					else if(memcmp(Usart1_Command2,Usart1_RxBuffer,8)==0) //查询湿度
					{
						retvalue2[0]=Device_Address;	
						retvalue2[3]=Humidity_HByte;
						retvalue2[4]=Humidity_LByte;
						Append_CRC16(retvalue2,5);
						for(i=0;i<7;i++)
						{
							USART_SendData(USART1,retvalue2[i]);
							while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
						}
					}
					else if(memcmp(Usart1_Command3,Usart1_RxBuffer,8)==0) //查询温湿度
					{
						retvalue3[0]=Device_Address;	
						retvalue3[3]=Temperature_HByre;
						retvalue3[4]=Temperature_LByre;
						retvalue3[5]=Humidity_HByte;
						retvalue3[6]=Humidity_LByte;
						Append_CRC16(retvalue3,7);
						for(i=0;i<9;i++)
						{
							USART_SendData(USART1,retvalue3[i]);
							while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
						}
					}
					else if(memcmp(Usart1_Command4,Usart1_RxBuffer,8)==0)
					{
						retvalue4[4]=Device_Address;
						Append_CRC16(retvalue4,5);
						for(i=0;i<7;i++)
						{
							USART_SendData(USART1,retvalue4[i]);
							while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
						}
					}
					else if(memcmp(Usart1_Command5,Usart1_RxBuffer,4)==0)
					{
						Usart1_Command5[5]=Usart1_RxBuffer[5];
						Append_CRC16(Usart1_Command5,6);
						if(memcmp(Usart1_Command5,Usart1_RxBuffer,8)==0)
						{
							Flash_Clear(Flash_Address);
							Flash_Write(Flash_Address,Usart1_RxBuffer[5]);
							if(Flash_Read(Flash_Address)==Usart1_RxBuffer[5])
							{
								Device_Address=Usart1_RxBuffer[5];
								for(i=0;i<8;i++)
								{
									USART_SendData(USART1,Usart1_RxBuffer[i]);
									while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
								}
								Usart1_Command1[0]=Device_Address;          
								Append_CRC16(Usart1_Command1,6);
								Usart1_Command2[0]=Device_Address;
								Append_CRC16(Usart1_Command1,6);
								Usart1_Command3[0]=Device_Address;
								Append_CRC16(Usart1_Command3,6);
							}
					}
					}
					memset(Usart1_RxBuffer,0,10);                     	//清空Usart接受缓冲区 缓冲区下标复位
					Number=0;
		}
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

