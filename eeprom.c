#include "eeprom.h"

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<1)); // EEWE == 1 
    /* Set up address and data registers */
    EEAR = uiAddress;
    EEDR = ucData;
    /* Write logical one to EEMWE */
    EECR |= (1<<2); // EEMWE == 2
    /* Start eeprom write by setting EEWE */
    EECR |= (1<<1); // EEWE == 1
}

void EEPROM_write_int(unsigned int uiAddress, int data)
{
    EEPROM_write(uiAddress, (unsigned char)(data & 0xFF));
    EEPROM_write(uiAddress + 1, (unsigned char)((data >> 8) & 0xFF));
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<1));
    /* Set up address register */
    EEAR = uiAddress;
    /* Start eeprom read by writing EERE */
    EECR |= (1<<0); // EERE == 0
    /* Return data from data register */
    return EEDR;
}

int EEPROM_read_int(unsigned int uiAddress)
{
    int data = 0;
    data = EEPROM_read(uiAddress);
    data |= (EEPROM_read(uiAddress + 1) << 8);
    return data;
}
