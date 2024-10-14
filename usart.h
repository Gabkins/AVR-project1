#ifndef USART_H
#define USART_H

#define F_CPU 11059200
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#define UCHAR unsigned char
#define TR PORTE_PORTE2
extern bool packet_ready;

void usart_setup(unsigned char ubrr);


void USART_Transmit(unsigned char data);

unsigned char USART_Receive(void);



#endif //  USART_H
