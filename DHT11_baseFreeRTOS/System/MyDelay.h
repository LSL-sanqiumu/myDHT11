#ifndef __MYDELAY_H
#define __MYDELAY_H

void MyDelay_Init(uint8_t SYSCLK);
void MyDelay_us(uint32_t nus);
void MyDelay_xms(uint32_t nms);

/* ������������� */
void MyDelay_ms(uint32_t nms);

#endif
