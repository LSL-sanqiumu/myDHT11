#ifndef __DHT11_H
#define __DHT11_H

#define DHT11_ACK_YES  1 
#define DHT11_ACK_NO   0

#define DHT11_SDA_HEIGH  1 
#define DHQ11_SDA_LOW   0

void DHT11_IO_Init(void);
uint8_t DHT11_CheckAck(void);
uint8_t DHT11_ReceiveDataByte(void);
uint8_t DHT11_GetData(uint8_t* temp, uint8_t* humi, uint8_t* decimal);

#endif
