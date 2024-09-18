#include "usart.h"
#include "crc.h"
#include "protocol.h"
#include "data.h"

//#define TR PORTE_PORTE2
#define USHRT unsigned short
#define UCHAR unsigned char

extern bool sendFlag = false;


void receivePacket() {
    unsigned char packet[6];


    for (int i = 0; i < 6; i++) {
        packet[i] = USART_Receive();
    }

    unsigned char crc = CRC8(packet, 5);
    
    if (packet[5] == 0x3F){

      USART_Transmit(crc);
    }
    
    if (crc == packet[5]) {
        unsigned char command = packet[0];
        unsigned short data = (packet[1] - '0') * 1000 + (packet[2] - '0') * 100 + (packet[3] - '0') * 10 + (packet[4] - '0'); 

        if (command == 0x72) {
            //unsigned char Freq[4];
            //sendPacket(0x72); //send Frequency
          sendFlag = true;
        } else if (command == 0x77) { //w (write) = 0x77
            writeData(data); 
        }
    }
}
