#include "stm32f10x.h"  

#include "FreeRTOS.h"
#include "task.h"

static uint8_t fac_us = 0;
static uint16_t fac_ms = 0;

/* SysTick_Handler的宏定义，SysTick_Handler已经被注释掉了,
xPortSysTickHandler具体实现在port.c */
extern void xPortSysTickHandler(void);

/* SysTick中断服务函数,使用OS时用到 */
/* 使用xTaskGetSchedulerState需要在FreeRTOS.h中的以下配置改为1
   #ifndef INCLUDE_xTaskGetSchedulerState
	#define INCLUDE_xTaskGetSchedulerState 1
   #endif 
*/

void SysTick_Handler(void)
{	
    /* 如果系统已经运行,就执行xPortSysTickHandler() */
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        /* 调用port.c已经实现了的SysTick中断函数 */
        xPortSysTickHandler();	
    }
}    
/* 设置滴答定时器的时钟来源 */
void MyDelay_Init(uint8_t SYSCLK)
{
	uint32_t reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); 
	fac_us = SYSCLK;							
	reload = SYSCLK;							/* 每一秒的计数次数 */	   
    /*根据delay_ostickspersec设定溢出时间
	  reload为24位寄存器,最大值为16777216,
      在168M下,计时最大值约为99864us
      在72MHz下,计时最大值约为233015us
    */
	reload *= 1000000 / configTICK_RATE_HZ;		
	fac_ms = 1000 / configTICK_RATE_HZ;			// 代表OS可以延时的最少单位	   
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // 开启SYSTICK中断
	SysTick->LOAD = reload; 					// 每1/configTICK_RATE_HZ进中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   // 开启SYSTICK     
}	    

void MyDelay_us(uint32_t nus)
{		
	uint32_t ticks = 0;
	uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    
	uint32_t reload = SysTick->LOAD;		    //LOAD的值	    	 
	ticks = nus * fac_us; 						//需要的节拍数 
	told = SysTick->VAL;        				//刚进入时的计数器值
    
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told)
                tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt += reload - tnow + told;	    
			told = tnow;
			if(tcnt >= ticks)
                break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}								    
} 
/* 会引起任务调度 */
void MyDelay_ms(uint32_t nms)
{	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)//系统已经运行
	{		
		if(nms >= fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(nms / fac_ms);	 		//FreeRTOS延时
		}
		nms %= fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	MyDelay_us((uint32_t)(nms * 1000));				//普通方式延时
}

/* 延时nms,不会引起任务调度 */
void MyDelay_xms(uint32_t nms)
{
	uint32_t i;
	for(i=0;i < nms; i++) 
        MyDelay_us(1000);
}



