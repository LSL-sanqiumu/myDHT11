#include "stm32f10x.h"                  // Device header

uint8_t tim1_delay_flag = 0;

void TIM1_Delay_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;  /*  */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1; /* T: 1us */
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
    
    TIM_ClearFlag(TIM1,TIM_IT_Update); // 避免刚初始化就进中断
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;   //  选择通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        // 启用
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应优先级
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM1, DISABLE);
}

void TIM1_Delay_us(uint16_t xus)
{
    TIM_SetCounter(TIM1, 0);
    TIM_SetAutoreload(TIM1, xus - 1);
    TIM_Cmd(TIM1, ENABLE);
    while(tim1_delay_flag == 0);
    tim1_delay_flag = 0;
    TIM_Cmd(TIM1, DISABLE);
}

void TIM1_Delay_ms(uint16_t xms)
{
    while(xms--){
        TIM1_Delay_us(1000);
    }
}

void TIM1_Delay_s(uint16_t xs)
{
    while(xs--){
        TIM1_Delay_ms(1000);
    }
}
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_FLAG_Update)){
       tim1_delay_flag = 1;
       TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
    }
}


