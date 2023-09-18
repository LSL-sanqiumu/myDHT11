#ifndef __DHT11_H__
#define __DHT11_H__

void DHT11_Init(void);
uint8_t DHT11_GetData(uint8_t* temp, uint8_t* humi, uint8_t* decimal);


#endif
