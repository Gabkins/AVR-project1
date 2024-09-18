#ifndef EEPROM_H
#define EEPROM_H

#include <ioavr.h>


void EEPROM_write(unsigned int uiAddress, unsigned char ucData);


void EEPROM_write_int(unsigned int uiAddress, int data);
 

unsigned char EEPROM_read(unsigned int uiAddress);

int EEPROM_read_int(unsigned int uiAddress);

#endif // EEPROM_H
