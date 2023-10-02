#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "OLED.h"
#include "MyDelay.h"
#include "DHT11.h"
#include "SerialLog.h"
#include "ESP8266.h"
#include "Timer.h"

void OLED_ShowInit(void)
{
    OLED_ShowString(1, 1, "DHT11&ESP8266");
    OLED_ShowString(2, 1, "temp:");
    OLED_ShowString(2, 9, ".");
    OLED_ShowString(3, 1, "humi:");
    OLED_ShowString(3, 9, "%");
}

static void STM32F103_HardwareInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    MyDelay_Init(72);
    OLED_Init();
    OLED_ShowInit();
    DHT11_IO_Init();
    
    TIM2_Init();
    TIM3_Init();
    ESP8266_SerialInit();
	SerialLog_Init();
	SerialLog_SendStrData("hardware init finish!\n");
    
}

#define DHT11_DataReadTask_StackDepth 512
#define OLED_DataShowTask_StackDepth 512
#define ESP8266_InitTask_StackDepth 512
#define ESP8266_DataUploadTask_StackDepth 512

#define DHT11_DataReadTask_Priority 2
#define OLED_DataShowTask_Priority 3
#define ESP8266_InitTask_Priority 4
#define ESP8266_DataUploadTask_Priority 3

#define DHT11Data_Queue_LEN 3
#define DHT11Data_Queue_SIZE 1

QueueHandle_t DHT11Data_Queue = NULL;
QueueHandle_t TempData_Queue = NULL;
QueueHandle_t DecimalData_Queue = NULL;
QueueHandle_t HumiData_Queue = NULL;

SemaphoreHandle_t Upload_Sem = NULL;

EventGroupHandle_t Event_Handle = NULL;


/* 定义两个事件 */
#define ESP8266Init_EVENT (0x01 << 0) // 位0

void DHT11_DataReadTask(void);
void OLED_DataShowTask(void);

void ESP8266_InitTask(void);
void ESP8266_DataUploadTask(void);

extern char buff[BUFF_MAX_SIZE];
extern uint8_t receive_flag;

extern uint8_t send_data_start;

char upload_cmd2[128] = 
{'A','T','+','M','Q','T','T','P','U','B','=','0',',',
'"','M','y','M','s','g','"',',',
'"','{','\\','"','t','e','m','p','\\','"',':','\\','"','3','6','.','0','\\','"','\\',',',
'\\','"','h','u','m','i','\\','"',':','\\','"','6','6','\\','"','}','"',',',
'0',',','0',
'\0'
};

int main(void) 
{
    STM32F103_HardwareInit();
    
    BaseType_t xReturn = pdTRUE;
    
    taskENTER_CRITICAL();
    
    DHT11Data_Queue = xQueueCreate(DHT11Data_Queue_LEN, DHT11Data_Queue_SIZE);
    TempData_Queue = xQueueCreate(1, 1);
    DecimalData_Queue = xQueueCreate(1, 1);
    HumiData_Queue = xQueueCreate(1, 1);
    
    Upload_Sem = xSemaphoreCreateBinary();
    
    Event_Handle = xEventGroupCreate();
    xEventGroupSetBits(Event_Handle, ESP8266Init_EVENT);
    
    xReturn = xTaskCreate((TaskFunction_t)DHT11_DataReadTask, 
        "DHT11_DataReadTask", 
        DHT11_DataReadTask_StackDepth, 
        NULL, 
        DHT11_DataReadTask_Priority, 
        NULL);
    xReturn = xTaskCreate((TaskFunction_t)OLED_DataShowTask, 
        "OLED_DataShowTask", 
        OLED_DataShowTask_StackDepth, 
        NULL, 
        OLED_DataShowTask_Priority, 
        NULL);
    xReturn = xTaskCreate((TaskFunction_t)ESP8266_InitTask, 
        "ESP8266_InitTask", 
        ESP8266_InitTask_StackDepth, 
        NULL, 
        ESP8266_InitTask_Priority, 
        NULL);
    xReturn = xTaskCreate((TaskFunction_t)ESP8266_DataUploadTask, 
        "ESP8266_DataUploadTask", 
        ESP8266_DataUploadTask_StackDepth, 
        NULL, 
        ESP8266_DataUploadTask_Priority, 
        NULL);
    
    taskEXIT_CRITICAL();
    if(xReturn){
        vTaskStartScheduler();
    }
	while(1);
}
void DHT11_DataReadTask(void)
{
    
    uint8_t temp;
    uint8_t humi;
    uint8_t decimal;
    while(1){
        if (DHT11_GetData(&temp, &humi, &decimal) == 1){
//          OLED_ShowNum(2,7,temp,2);
            xQueueSend(DHT11Data_Queue, &temp, portMAX_DELAY);
//            vTaskDelay(10);
//			OLED_ShowNum(2,10,decimal,1);
            xQueueSend(DHT11Data_Queue, &decimal, portMAX_DELAY);
//            vTaskDelay(10);
//			OLED_ShowNum(3,7,humi,2);
            xQueueSend(DHT11Data_Queue, &humi, portMAX_DELAY);
//            vTaskDelay(10);
            xQueueSend(TempData_Queue, &temp, portMAX_DELAY);
            xQueueSend(DecimalData_Queue, &decimal, portMAX_DELAY);
            xQueueSend(HumiData_Queue, &humi, portMAX_DELAY);
            
            xSemaphoreGive(Upload_Sem);
        }
        vTaskDelay(20);
    }
}
void OLED_DataShowTask(void)
{
    BaseType_t xReturn = pdPASS;
    uint8_t receive_buff = 0;
    uint8_t data_index = 0;
    while(1){
           
        xReturn = xQueueReceive(DHT11Data_Queue, &receive_buff, portMAX_DELAY);
        if(xReturn == pdPASS){
            data_index++;
        }
        if(data_index == 1) {
            OLED_ShowNum(2,7,receive_buff,2);
        }else if(data_index == 2) {
            OLED_ShowNum(2,10,receive_buff,1);
        }else if(data_index == 3) {
            data_index = 0;
            OLED_ShowNum(3,7,receive_buff,2);
        }
        
        vTaskDelay(20);
    }
}

void ESP8266_InitTask(void)
{
    EventBits_t r_event;
    while(1){
        r_event = xEventGroupWaitBits(Event_Handle, ESP8266Init_EVENT,
            pdTRUE, pdTRUE, portMAX_DELAY);
        if((r_event & (ESP8266Init_EVENT)) == ESP8266Init_EVENT){
            ESP8266_Init();
        }
        vTaskDelay(20);
    }
}
void ESP8266_DataUploadTask(void)
{
    BaseType_t xReturn = pdPASS;
    uint8_t temp = 0;
    uint8_t decimal = 0;
    uint8_t humi = 0;
    while(1){
        
        xReturn = xSemaphoreTake(Upload_Sem, portMAX_DELAY);
        
        if(xReturn == pdPASS){
            
            xQueueReceive(TempData_Queue, &temp, portMAX_DELAY);
            xQueueReceive(DecimalData_Queue, &decimal, portMAX_DELAY);
            xQueueReceive(HumiData_Queue, &humi, portMAX_DELAY);
            
            ESP8266_PublishedTempAndHumi(upload_cmd2,temp,decimal,humi);
            
            if(receive_flag == 1){
                receive_flag = 0;
                if(ESP8266_HasTargetInfo("ERROR") || ESP8266_HasTargetInfo("wdt reset")){
                    xEventGroupSetBits(Event_Handle, ESP8266Init_EVENT);
                }
                SerialLog_SendStrData(buff);
            }    
        }
        
        vTaskDelay(1000);
    }
}


