#ifndef __ESP8266_H__
#define __ESP8266_H__

#define BUFF_MAX_SIZE 1024

#define TRUE      0x01
#define FALSE     0x00

#define CONNECTED      TRUE
#define DISCONNECT     FALSE

/* 指令应答信号 */
#define ACK_SUCCESS 0x01
#define ACK_FAIL    0x00
#define ACK_NONE    0x02

#define AT_YES    0x01
#define AT_NO     0x00

/** WiFi连接状态：处于状态1、2说明已经连接上，状态0、4说明没连接上 **/
#define WIFI_STATE_0 0x10 /* 未连接过AP */
#define WIFI_STATE_1 0x11 /* 已连接AP，但未获取到IPv4地址 */
#define WIFI_STATE_2 0x12 /* 已连接AP，并获取到IPv4地址 */
#define WIFI_STATE_3 0x13 /* 正在进行WiFi连接或重连 */
#define WIFI_STATE_4 0x14 /* 连接过AP，但现在处于WiFi断开状态 */

/** AT指令- 基础AT指令 **/
#define AT "AT"
#define AT_RESTART "AT+RST"
#define AT_QUERY_VERSION "AT+GMR"

/* AT指令- WiFi-AT指令 */
#define WIFI_QUERY_INFO    "AT+CWJAP?"   /* 查看连接上的的WiFi的信息 */
#define WIFI_QUERY_STATE   "AT+CWSTATE?" /* 查看WiFi连接状态 */
#define WIFI_CONNECT       "AT+CWJAP=\"quiet\",\"19990903\"" /* 连接WiFi */
#define WIFI_DISCONNECT    "AT+CWQAP" /* 断开连接 */

/** AT指令- MQTT-AT指令 **/
/* 用户属性设置 */
#define MQTT_SET_USER_ATTR "AT+MQTTUSERCFG=0,1,\"ESP826601S\",\
\"admin\",\"isMQTT20230919\",0,0,\"\"" 
/* 设置为不自动重连，当MQTT断开后需要先 执行MQTT_DISCONNECT再执行MQTT_SET_USER_ATTR和MQTT_CONNECT*/
#define MQTT_CONNECT       "AT+MQTTCONN=0,\"175.178.181.190\",1883,0"
#define MQTT_DISCONNECT    "AT+MQTTCLEAN=0" /* 断开与MQTT服务器的连接 */
/* AT+MQTTPUB=<LinkID>,<"topic">,<"data">,<qos>,<retain> 向主题topic发送数据data，注意该命令不会发送'\0' */
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
