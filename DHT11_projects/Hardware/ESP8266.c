#include "stm32f10x.h"                  // Device header

#include <string.h>
#include "Delay.h"
#include "ESP8266.h"
#include "SerialTest.h"

char buff[BUFF_MAX_SIZE];
uint16_t buff_index;

uint8_t receive_flag;
uint8_t buff_init_flag;

char upload_cmd[256] = "AT+MQTTPUB=0,\"DHT11\",\"esp hello\",0,0";

/* PA2-TX  PA3-RX */

void ESP8266_SerialInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART2, ENABLE);
}

void Serial_SendData(uint16_t data)
{

	USART_SendData(USART2, data);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
}
void Serial_SendStrData(char* str)
{
	uint16_t i;
	for(i = 0; str[i] != '\0'; i++){
		Serial_SendData(str[i]);
	}
}

void Serial_SendCommand(char* cmd)
{
	Serial_SendStrData(cmd);
	Serial_SendData('\r');
	Serial_SendData('\n');
    Delay_ms(20);
}
void USART2_IRQHandler(void)
{
	static uint16_t index = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
        if(buff_init_flag){
            index = 0;
            buff_init_flag = 0;
        }
        if(index < BUFF_MAX_SIZE) {
            buff[index++] = USART_ReceiveData(USART2);
            buff_index = index;
            TIM_SetCounter(TIM2, 0);
            TIM_Cmd(TIM2, ENABLE);
        }else {
            
        }
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
/*
**********************************************************
*������ �� ESP8266_HasTargetInfo()
*��  �� �� �ж���Ӧ��Ϣ���Ƿ���Ŀ����Ϣ
*��  �� �� ��
*��  �� �� TRUE������ FALSE������
*/
uint8_t ESP8266_HasTargetInfo(char* info)
{
    
    if(strstr(buff, info)){
        return TRUE;
    }
    return FALSE;
}
/*********************************************************/
/*********************************************************/
/*********************************************************/
/*********************************************************/
/*********************************************************/

/*
**********************************************************
*������ �� ESP8266_ACK()
*��  �� �� �ж�ָ���Ƿ�ִ�гɹ�
*��  �� �� ��
*��  �� �� ACK_FAIL-0������ָ���ACK_SUCCESS-1���ɹ�ʶ��ִ�У���ACK_NONE-2������Ӧ��
*/
uint8_t ESP8266_ACK(void)
{
    
    if(ESP8266_HasTargetInfo("ERROR") == TRUE){
        return ACK_FAIL;
    }else if(ESP8266_HasTargetInfo("OK") == TRUE){
        return ACK_SUCCESS;
    }
    
    return ACK_NONE;
}
/*
**********************************************************
*������ �� ESP8266_ATIsOK()
*��  �� �� �ж�ATָ���Ƿ����ʹ��
*��  �� �� ��
*��  �� �� 
*/
uint8_t ESP8266_ATIsOK(void)
{
    Serial_SendCommand(AT);
    Delay_ms(20);
    if(receive_flag == 1){
        if(ESP8266_ACK() == ACK_SUCCESS) {
            receive_flag = 0;
            return AT_YES;
        }
    }
    return AT_NO;
}
/*
**********************************************************
*������ �� ESP8266_Restart()
*��  �� �� ����ESP8266
*��  �� �� ��
*��  �� �� ��
*/
void ESP8266_Restart(void)
{
    Serial_SendCommand(AT_RESTART);
    Delay_ms(1000);
    SerialTest_SendStrData("ESP8266: restart ===> ......OK!\n");
}
/*
**********************************************************
*������ �� ESP8266_WiFiConnected()
*��  �� �� ���WiFi����״̬
*��  �� �� ��
*��  �� �� WIFI_STATE_0 ���� WIFI_STATE_4
*/
uint8_t ESP8266_QueryWiFiState(void)
{
    Serial_SendCommand(WIFI_QUERY_STATE);
    Delay_ms(20);
    
    if(receive_flag == 1){
        if(ESP8266_ACK() == ACK_SUCCESS) {
            receive_flag = 0;
            
            if(ESP8266_HasTargetInfo("+CWSTATE:0,") == TRUE){
                return WIFI_STATE_0;
            }
            if(ESP8266_HasTargetInfo("+CWSTATE:1,") == TRUE){
                return WIFI_STATE_1;
            }
            if(ESP8266_HasTargetInfo("+CWSTATE:2,") == TRUE){
                return WIFI_STATE_2;
            }
            if(ESP8266_HasTargetInfo("+CWSTATE:3,") == TRUE){
                return WIFI_STATE_3;
            }
            if(ESP8266_HasTargetInfo("+CWSTATE:4,") == TRUE){
                return WIFI_STATE_4;
            }
        }
        return ACK_FAIL;
    }
    return ACK_NONE;
}

uint8_t ESP8266_WiFiConnected(void)
{
    uint8_t status = ESP8266_QueryWiFiState();
    Delay_ms(20);
    if((status == WIFI_STATE_1) || (status == WIFI_STATE_2)) {
        return CONNECTED;
    }
    return DISCONNECT;
}
uint8_t ESP8266_ConnectWiFi(void)
{
    if(ESP8266_WiFiConnected() == CONNECTED) {
        SerialTest_SendStrData("ESP8266: wifi connected!\n");
        return TRUE;
    }else {
        SerialTest_SendStrData("ESP8266: connect wifi ===> ");
        Serial_SendCommand(WIFI_CONNECT);
        Delay_s(1);
        
        if(receive_flag == 1){
            receive_flag = 0;
            if(ESP8266_HasTargetInfo("WIFI CONNECTED")){
                SerialTest_SendStrData("......OK!\n");
                return TRUE;
            }else {
                SerialTest_SendStrData("......FAIL!Start reconnecting......\n");
                return ESP8266_ConnectWiFi();
            }
        }
    }
    return FALSE;
}
/*
void ESP8266_WiFiReconnecting(void)
{
    
}
*/
uint8_t ESP8266_MQTTConnected(void)
{
    if(ESP8266_WiFiConnected() == CONNECTED){
        Serial_SendCommand(MQTT_QUERY_STATE);
        Delay_ms(100);
        if(receive_flag == 1){
            receive_flag = 0;
            if(ESP8266_ACK() == ACK_SUCCESS) {
                if(ESP8266_HasTargetInfo("+MQTTCONN:0,0,0,")){
                    SerialTest_SendStrData("ESP8266: MQTT not connected!\n");
                    return DISCONNECT;
                }else {
                    SerialTest_SendStrData("ESP8266: MQTT connected!\n");
                    return CONNECTED;
                }
            }else {
                SerialTest_SendStrData("ESP8266: MQTT query --- error!\n");
                return DISCONNECT;
            }
        }
    }
    return DISCONNECT;
}
void ESP8266_ConnectMQTT(void)
{
    while(ESP8266_MQTTConnected() == DISCONNECT){
        SerialTest_SendStrData("ESP8266: MQTT reconnecting......!\n");
        Serial_SendCommand(MQTT_DISCONNECT);
        Delay_ms(100);
        Serial_SendCommand(MQTT_SET_USER_ATTR);
        Delay_ms(100);
        Serial_SendCommand(MQTT_CONNECT);
        Delay_ms(100);
    }
//    SerialTest_SendStrData("ESP8266: MQTT connected!\n");
}

/*
**********************************************************
*������ �� ESP8266_Init()
*��  �� �� ESP8266��ʼ��
*��  �� �� ��
*��  �� �� 0��1
*/
void ESP8266_Init(void)
{
    /* ����ESP8266 */
    ESP8266_Restart();
    /* ����WiFi */
    ESP8266_ConnectWiFi();
    /* ����MQTT */
    ESP8266_ConnectMQTT();
}

void ESP8266_PublishedData(void)
{
    
    Serial_SendCommand(upload_cmd);
}

void test(void)
{
    ESP8266_Init();
    ESP8266_PublishedData();
}

