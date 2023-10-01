#ifndef __SERIAL_LOG_H
#define __SERIAL_LOG_H



void SerialLog_Init(void);
void SerialLog_SendData(uint16_t data);
void SerialLog_SendStrData(char* str);


#endif
