#include "stm32f10x.h"                  // Device header

#include "OLED.h"
#include "Delay.h"
#include "DHT11.h"
#include "ESP8266.h"
#include "SerialTest.h"
#include "Timer.h"
#include "string.h"
#include "Key.h"

uint8_t temp;
uint8_t humi;
uint8_t decimal;

extern char buff[BUFF_MAX_SIZE];
extern uint8_t receive_flag;

char wifi_name[128] = "quiet";
char wifi_passwd[64] = "19990903"; /* 密码最长为63字节ASCII码 */

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    OLED_Init();
	DHT11_Init();
    TIM2_Init();
    Key_Init();
    OLED_ShowString(1, 1, "DHT11&ESP8266");
    OLED_ShowString(2, 1, "temp:");
    OLED_ShowString(2, 9, ".");
    OLED_ShowString(3, 1, "humi:");
    OLED_ShowString(3, 9, "%");
	ESP8266_SerialInit();
	SerialTest_Init();
	SerialTest_SendStrData("DHT11 init\n");
    
    test();
    
	while(1){
		if(DHT11_GetData(&temp, &humi, &decimal)){
			OLED_ShowNum(2,7,temp,2);
			OLED_ShowNum(2,10,decimal,1);
			OLED_ShowNum(3,7,humi,2);
		}
		if(Key_Scan(GPIOA, GPIO_Pin_8)){
			Serial_SendCommand(AT);
            test();
		}
//		if(receive_flag == 1){
//            if(strstr(buff,"ERROR")){
//                OLED_ShowString(4, 1, "ERROR!");
//            }else if (strstr(buff,"OK")){
//                OLED_ShowString(4, 1, "      ");
//                OLED_ShowString(4, 1, "OK!");
//            }
//			receive_flag = 0;
//			SerialTest_SendStrData(buff);
//		}
		DHT11_GetData(&temp, &humi, &decimal);
		OLED_ShowNum(2,7,temp,2);
		OLED_ShowNum(2,10,decimal,1);
		OLED_ShowNum(3,7,humi,2);
		

	}
	

}
