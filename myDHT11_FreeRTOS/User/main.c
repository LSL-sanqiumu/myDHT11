#include "stm32f10x.h"                  // Device header
#include "OLED.h"

#include "FreeRTOS.h"
#include "task.h"

static void STM32F103_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

int main(void) 
{
	while(1){

	}
}

