#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;	
    
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{	
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0){
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0);
		Delay_ms(10);
		return 1;
	}
	return 0;
}


