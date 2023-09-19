#ifndef __ESP8266_H__
#define __ESP8266_H__

#define BUFF_MAX_SIZE 1024

#define TRUE      0x01
#define FALSE     0x00

#define CONNECTED      TRUE
#define DISCONNECT     FALSE

/* ָ��Ӧ���ź� */
#define ACK_SUCCESS 0x01
#define ACK_FAIL    0x00
#define ACK_NONE    0x02

#define AT_YES    0x01
#define AT_NO     0x00

/** WiFi����״̬������״̬1��2˵���Ѿ������ϣ�״̬0��4˵��û������ **/
#define WIFI_STATE_0 0x10 /* δ���ӹ�AP */
#define WIFI_STATE_1 0x11 /* ������AP����δ��ȡ��IPv4��ַ */
#define WIFI_STATE_2 0x12 /* ������AP������ȡ��IPv4��ַ */
#define WIFI_STATE_3 0x13 /* ���ڽ���WiFi���ӻ����� */
#define WIFI_STATE_4 0x14 /* ���ӹ�AP�������ڴ���WiFi�Ͽ�״̬ */

/** ATָ��- ����ATָ�� **/
#define AT "AT"
#define AT_RESTART "AT+RST"
#define AT_QUERY_VERSION "AT+GMR"

/* ATָ��- WiFi-ATָ�� */
#define WIFI_QUERY_INFO    "AT+CWJAP?"   /* �鿴�����ϵĵ�WiFi����Ϣ */
#define WIFI_QUERY_STATE   "AT+CWSTATE?" /* �鿴WiFi����״̬ */
#define WIFI_CONNECT       "AT+CWJAP=\"quiet\",\"19990903\"" /* ����WiFi */
#define WIFI_DISCONNECT    "AT+CWQAP" /* �Ͽ����� */

/** ATָ��- MQTT-ATָ�� **/
/* �û��������� */
#define MQTT_SET_USER_ATTR "AT+MQTTUSERCFG=0,1,\"ESP826601S\",\
\"admin\",\"isMQTT20230919\",0,0,\"\"" 
/* ����Ϊ���Զ���������MQTT�Ͽ�����Ҫ�� ִ��MQTT_DISCONNECT��ִ��MQTT_SET_USER_ATTR��MQTT_CONNECT*/
#define MQTT_CONNECT       "AT+MQTTCONN=0,\"175.178.181.190\",1883,0"
#define MQTT_DISCONNECT    "AT+MQTTCLEAN=0" /* �Ͽ���MQTT������������ */
/* AT+MQTTPUB=<LinkID>,<"topic">,<"data">,<qos>,<retain> ������topic��������data��ע�������ᷢ��'\0' */
#define MQTT_PUBLISH_STR   "AT+MQTTPUB=0,\"MyMsg\",\"{\\\"esp\\\":\\\"36\\\"}\",0,0" /*  */
#define MQTT_QUERY_STATE   "AT+MQTTCONN?"
#define MQTT_SUBSCRIBE_TOPIC "AT+MQTTSUB=0,\"ConnectedFlag\",1"

void ESP8266_SerialInit(void);
void Serial_SendData(uint16_t data);
void Serial_SendStrData(char* str);
void Serial_SendCommand(char* cmd);

void ESP8266_Init(void);

void ESP8266_PublishedData(char* data);

void ESP8266_PublishedTempAndHumi(char* upload_cmd,uint8_t temp, uint8_t decimal, uint8_t humi);

void ESP8266_ReceiveTopicData(void);

uint8_t ESP8266_HasTargetInfo(char* info);
void test(void);

#endif
