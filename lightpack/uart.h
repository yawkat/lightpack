//
// Created by yawkat on 7/28/18.
//

#ifndef LIGHTPACK_UART_H
#define LIGHTPACK_UART_H

#include <avr/io.h>

void USART_Init(void);

uint8_t USART_ReceiveByte();

void USART_SendByte(uint8_t u8Data);

#endif //LIGHTPACK_UART_H
