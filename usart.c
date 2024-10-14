#include <ioavr.h>
#include <intrinsics.h>
#include <ina90.h>
#include <stdio.h>
#include <ina90.h>

#include "usart.h"
#include "crc.h"
#include "data.h"

#define UCHAR unsigned char
#define TR PORTE_PORTE2
#define DELAY_US(us) 	__delay_cycles((F_CPU / 1000000) * (us))


unsigned char packet[6];
unsigned char Spacket[5];
unsigned short packet_index_rx = 0;
unsigned short packet_index_tx = 0;

extern bool packet_ready = false;
extern bool sendFlag = false;

void usart_setup(UCHAR ubrr)
{
  UBRR0H = (UCHAR)(ubrr >> 8);
  UBRR0L = (UCHAR)ubrr;
  
  UCSR0B_RXCIE0 = 1; //recieve interrupt 
  //UCSR0B_UDRIE0 = 1; //transmit interrupt
  UCSR0B_RXEN0 = 1;  //recieve permission
  UCSR0B_TXEN0 = 1; //transmit permission
  
  UCSR0C_UCSZ00 = 1; 
  UCSR0C_UCSZ01 = 1;
  
}

void receivePacket() {

    packet_ready = false;
    unsigned char crc = CRC8(packet, 5);
    
    /*if (packet[5] == 0x3F){

      USART_Transmit(crc);
    }*/
    
    if (crc == packet[5]) {
        unsigned char command = packet[0];
        unsigned short data = (packet[1] - '0') * 1000 + (packet[2] - '0') * 100 + (packet[3] - '0') * 10 + (packet[4] - '0'); 

        if (command == 0x72) { //r

          Spacket[0] = Frequency / 1000 + '0';  // 
          Spacket[1] = Frequency / 100 % 10 + '0';
          Spacket[2] = Frequency / 10 % 10 + '0';
          Spacket[3] = Frequency % 10 + '0';
          Spacket[4] = ' ';
          packet_index_tx = 0;
          //sendFlag = true;
          //Spacket;
          TR = 1;
          DELAY_US(60);
          UCSR0B_UDRIE0 = 1;
          UDR0 = Spacket[packet_index_tx++];
        } else if (command == 0x77) { //w (write) = 0x77
            writeData(data); 
        }
    }

}

#pragma vector=USART0_RXC_vect
__interrupt void ISR_USART0_RX(void) {
  
  
  
  packet[packet_index_rx++] = UDR0;

  if (packet_index_rx == 6) {
  
    packet_index_rx = 0;        
    packet_ready = true;

  }
  
}

#pragma vector=USART0_UDRE_vect
__interrupt void ISR_USART0_UDRE(void)
{

  if (packet_index_tx < 5)
  {
    DELAY_US(100);
    UDR0 = Spacket[packet_index_tx++];
 
  } else { 
    UCSR0B_UDRIE0 = 0;
    UCSR0B_TXCIE0 = 1; 
    //UDR0 = Spacket[packet_index_tx++];
    //packet_index_tx = 0;
    
    
  }
  
}

#pragma vector=USART0_TXC_vect
__interrupt void ISR_USART0_TX(void)
{
  TR = 0;
  UCSR0B_TXCIE0 = 0; //recieve interrup
}