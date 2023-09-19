#include "stm32f10x.h"                  // Device header

#include "ESP8266.h"


extern char buff[BUFF_MAX_SIZE];
extern uint16_t buff_index;

extern uint8_t receive_flag;
extern uint8_t buff_init_flag;

uint8_t send_data_start;


void TIM2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 200 - 1; /* 20ms */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef  NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_InitStruct);
    
    TIM_Cmd(TIM2, DISABLE);
    
}
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET){
        
        receive_flag = 1;
        buff_init_flag = 1;
        buff[buff_index] = '\0';
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        
        TIM_Cmd(TIM2, DISABLE);
    }
}

void TIM3_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1; /* 10000 - 1000ms */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
    
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef  NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 4;
    NVIC_Init(&NVIC_InitStruct);
    
    TIM_Cmd(TIM3, DISABLE);
    
}
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
        
        send_data_start = 1;
        
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        TIM_Cmd(TIM3, DISABLE);
    }
}

