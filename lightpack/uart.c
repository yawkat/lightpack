//
// Created by yawkat on 7/28/18.
//

#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#if F_CPU == 8000000
#define UCSR0A UCSRA
#define UDR0 UDR
#define UDRE0 UDRE
#define UBRR0L UBRRL
#define UBRR0H UBRRH
#define U2X0 U2X
#define UCSR0B UCSRB
#define TXEN0 TXEN
#define RXEN0 RXEN
#define RXC0 RXC
#endif

void USART_SendByte(uint8_t u8Data) {
    // Wait until last byte has been transmitted
    while ((UCSR0A & (1 << UDRE0)) == 0);

    // Transmit data
    UDR0 = u8Data;
}

void USART_Init(void) {
    // Set baud rate

    if (F_CPU == 16000000) {
        UCSR0A |= 1 << U2X0;
        UBRR0L = 16;
        UBRR0H = 0;
    } else if (F_CPU == 8000000) {
        UCSR0A |= 1 << U2X0;
        UBRR0L = 8;
        UBRR0H = 0;
    }
    /* Load upper 8-bits into the high byte of the UBRR register
   Default frame format is 8 data bits, no parity, 1 stop bit
 to change use UCSRC, see AVR datasheet*/

    // Enable receiver and transmitter and receive complete interrupt
    UCSR0B = ((1 << TXEN0) | (1 << RXEN0));
}



// not being used but here for completeness
// Wait until a byte has been received and return received data
uint8_t USART_ReceiveByte() {
    while ((UCSR0A & (1 << RXC0)) == 0);
    return UDR0;
}