#include "stm32f10x.h"                  // Device header
#include "MyDelay.h"

#define DHT11_ACK_YES  1 
#define DHT11_ACK_NO   0

#define DHT11_SDA_HEIGH  1 
#define DHQ11_SDA_LOW   0

void DHT11_IO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_7);            
}

void DHT11_Start(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_7); // ÖÃµÍ
//	Delay_ms(20);                      // ³ÖÐø20ms
//    TIM1_Delay_ms(20);
    MyDelay_xms(20);
	GPIO_SetBits(GPIOA, GPIO_Pin_7);   // ÖÃ¸ß
}

uint8_t DHT11_CheckAck(void)
{
	DHT11_Start();
	uint8_t time = 0;
	
	while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == DHT11_SDA_HEIGH && (time < 100)){
		time++;
//		Delay_us(1);
//        TIM1_Delay_us(1);
        MyDelay_us(1);
	}
	if (time < 100){
		time = 0;
	}else {
		return DHT11_ACK_NO;
	}

	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == DHQ11_SDA_LOW && time < 100){
		time++;
//		Delay_us(1);
//        TIM1_Delay_us(1);
        MyDelay_us(1);
	}
	
	return  time < 100 ? DHT11_ACK_YES : DHT11_ACK_NO;
}


uint8_t DHT11_ReceiveDataBit(void)
{
	while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == DHT11_SDA_HEIGH){
//		Delay_us(1);
//		TIM1_Delay_us(1);
        MyDelay_us(1);
	}
	while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == DHQ11_SDA_LOW){
//		Delay_us(1);
//        TIM1_Delay_us(1);
        MyDelay_us(1);
	}
	
//	Delay_us(40);
//    TIM1_Delay_us(40);
    MyDelay_us(40);
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == DHT11_SDA_HEIGH;
}

uint8_t DHT11_ReceiveDataByte(void)
{
	uint8_t data_byte = 0;
	uint8_t i;
	for(i = 0; i < 8; ++i){
		data_byte = data_byte << 1;
		data_byte += DHT11_ReceiveDataBit();
	}
	return data_byte;
}


uint8_t DHT11_GetData(uint8_t* temp, uint8_t* humi, uint8_t* decimal)
{
	uint8_t buff[5];
	uint8_t i;
	if (DHT11_CheckAck() == DHT11_ACK_YES){
		for (i = 0; i < 5; ++i){
			buff[i] = DHT11_ReceiveDataByte();
		}
		if (buff[0] + buff[1] + buff[2] + buff[3] == buff[4]){
			*temp = buff[2];
			*humi = buff[0];
			*decimal = buff[3];
			return 1;
		}
		return 0;
	}
	return 0;
}

