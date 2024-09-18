#include <ioavr.h>
#include "usart.h"

#define UCHAR unsigned char
#define TR PORTE_PORTE2

void usart_setup(UCHAR ubrr)
{
  UBRR0H = (UCHAR)(ubrr >> 8);
  UBRR0L = (UCHAR)ubrr;
  
  UCSR0B_RXEN0 = 1;  //recieve permission
  UCSR0B_TXEN0 = 1; //transmit permission
  
  UCSR0C_UCSZ00 = 1; //8 ���, 1 ����-���
  UCSR0C_UCSZ01 = 1;
}

void USART_Transmit(UCHAR data )
{
   TR = 1;
/* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << 5))); //UDRE0
/* Put data into buffer, sends the data */
  UDR0 = data;

}


unsigned char USART_Receive( void )
{
  TR = 0;
/* Wait for data to be received */
  while (!(UCSR0A & (1 << 7))); //RXC0 == 7
  return UDR0;
/* Get and return received data from buffer */
   
}