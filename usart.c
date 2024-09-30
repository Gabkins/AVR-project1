#include <ioavr.h>
#include "usart.h"
#include "crc.h"
#include "data.h"

#define UCHAR unsigned char
#define TR PORTE_PORTE2

unsigned char packet[6];

unsigned short packet_index = 0;

bool packet_ready = false;
extern bool sendFlag = false;

void usart_setup(UCHAR ubrr)
{
  UBRR0H = (UCHAR)(ubrr >> 8);
  UBRR0L = (UCHAR)ubrr;
  
  UCSR0B_RXCIE0 = 1; //recieve interrupt 
  UCSR0B_RXEN0 = 1;  //recieve permission
  UCSR0B_TXEN0 = 1; //transmit permission
  
  UCSR0C_UCSZ00 = 1; 
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


/*unsigned char USART_Receive( void )
{
  TR = 0;
// Wait for data to be received 
  while (!(UCSR0A & (1 << 7))); //RXC0 == 7
  return UDR0;
//Get and return received data from buffer 
   
}*/


#pragma vector=USART0_RXC_vect

__interrupt void ISR_USART0_RX(void) {
  
  //unsigned char received = UDR0;
  
  packet[packet_index++] = UDR0;
  
  if (packet_index == 6) {
        
    packet_index = 0;        
    packet_ready = true;  
  }
  
}


void receivePacket() {

  if (packet_ready){
    packet_ready = false;
    unsigned char crc = CRC8(packet, 5);
    
    /*if (packet[5] == 0x3F){

      USART_Transmit(crc);
    }*/
    
    if (crc == packet[5]) {
        unsigned char command = packet[0];
        unsigned short data = (packet[1] - '0') * 1000 + (packet[2] - '0') * 100 + (packet[3] - '0') * 10 + (packet[4] - '0'); 

        if (command == 0x72) {
            //unsigned char  Freq[4];
            //sendPacket(0x72); //send Frequency
          sendFlag = true;
        } else if (command == 0x77) { //w (write) = 0x77
            writeData(data); 
        }
    }
  }
}