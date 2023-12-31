#include "stm32f10x.h"                  // Device header

#include "OLED.h"
#include "Delay.h"
#include "DHT11.h"
#include "ESP8266.h"
#include "SerialTest.h"
#include "Timer.h"
#include "string.h"
#include "Key.h"
#include "Util.h"

uint8_t temp;
uint8_t humi;
uint8_t decimal;

uint8_t old_temp;
uint8_t old_humi;
uint8_t old_decimal;

extern char buff[BUFF_MAX_SIZE];
extern uint8_t receive_flag;

extern uint8_t send_data_start;

char wifi_name[128] = "quiet";
char wifi_passwd[64] = "19990903"; /* 密码最长为63字节ASCII码 */

//char upload_cmd[256] = "AT+MQTTPUB=0,\"MyMsg\",\"{\\\"temp\\\":\\\"36\\\",\\\"humi\\\":\\\"66.0\\\"}\",0,0";
char upload_cmd[256] = "AT+MQTTPUB=0,\"MyMsg\",\"{\\\"temp\\\":\\\"36\\\"\\,\\\"humi\\\":\\\"66.0\\\"}\",0,0";
/* 可重写printf来格式化指令 */
char upload_cmd2[256] = 
{'A','T','+','M','Q','T','T','P','U','B','=','0',',',
'"','M','y','M','s','g','"',',',
'"','{','\\','"','t','e','m','p','\\','"',':','\\','"','3','6','.','0','\\','"','\\',',',
'\\','"','h','u','m','i','\\','"',':','\\','"','6','6','\\','"','}','"',',',
'0',',','0',
'\0'
};

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    OLED_Init();
	DHT11_Init();
    TIM2_Init();
    TIM3_Init();
    Key_Init();
    OLED_ShowString(1, 1, "DHT11&ESP8266");
    OLED_ShowString(2, 1, "temp:");
    OLED_ShowString(2, 9, ".");
    OLED_ShowString(3, 1, "humi:");
    OLED_ShowString(3, 9, "%");
	ESP8266_SerialInit();
	SerialTest_Init();
	SerialTest_SendStrData("hardware init finish!\n");
    
    ESP8266_Init();
    
//    DHT11_GetData(&old_temp, &old_humi, &old_decimal);
    
	while(1){
		if(DHT11_GetData(&temp, &humi, &decimal)){
			OLED_ShowNum(2,7,temp,2);
			OLED_ShowNum(2,10,decimal,1);
			OLED_ShowNum(3,7,humi,2);
		}
		if(Key_Scan(GPIOA, GPIO_Pin_8)){
            uint8_t j;
            char number;
            for(j = 0; j < 2; j++){
                number = temp / Serial_Pow(10, 2 - j - 1) % 10 + '0';
                upload_cmd2[34 + j] = number;
            }
            upload_cmd2[37] = decimal / Serial_Pow(10, 0) % 10 + '0';
            for(j = 0; j < 2; j++){
                number = humi / Serial_Pow(10, 2 - j - 1) % 10 + '0';
                upload_cmd2[53 + j] = number;
            }
            
            ESP8266_PublishedData(upload_cmd2);
		}
        
        TIM_Cmd(TIM3, ENABLE);
        if(send_data_start == 1){
            send_data_start = 0;
//            if((old_temp != temp) || (old_decimal != decimal) || (old_humi != humi)){
//                ESP8266_PublishedTempAndHumi(upload_cmd2,temp,decimal,humi);
//            }
//            old_temp = temp;
//            old_decimal = decimal;
//            old_humi = humi;
            ESP8266_PublishedTempAndHumi(upload_cmd2,temp,decimal,humi);
            if(receive_flag == 1){
                receive_flag = 0;
                if(ESP8266_HasTargetInfo("ERROR")){
                    ESP8266_Init();
                }
                SerialTest_SendStrData(buff);
            }
        }
	}
}
