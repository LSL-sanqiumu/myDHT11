#include "stm32f10x.h"                  // Device header

uint32_t Serial_Pow(uint32_t X, uint8_t Y)
{
	uint32_t Result = 1;
	while(Y--)	
		Result *= X;
	return Result;
}
/* 将数值转为字符形式的数值 */
//char Util_NumToChar(uint8_t num, uint8_t length)
//{
//    uint8_t i;
//    for(i = 0; i < length; i++){
//        return num / Serial_Pow(10, length - i - 1) % 10;
//    }
//}








