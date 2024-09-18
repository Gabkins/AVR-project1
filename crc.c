#include "crc.h"

unsigned char CRC8(unsigned char *data, unsigned char length) {
    unsigned char crc = 0x00;
    for (unsigned char i = 0; i < length; i++) {
        crc ^= data[i];
        for (unsigned char j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else { 
                crc <<= 1;
            }
        }
    }
    return crc;
}
