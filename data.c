#include "data.h"
#include "usart.h"
#include "eeprom.h"

#define F_CPUd1024 10800
#define TR PORTE_PORTE2

extern unsigned short Frequency = 10;
extern bool blinkFlag = false;


void setFrequency(unsigned short freq)
{ 
  unsigned int ocr_value = (unsigned long)((F_CPUd1024 / freq)*10 - 1);
  
  if(TCNT1 > ocr_value)
  {TCNT1 = ocr_value - 10;}
  OCR1A = ocr_value;

}

void writeData(unsigned short value) {
    if (value >= 1 && value <= 2000) {
        EEPROM_write_int(0, Frequency);

        Frequency = value;
        
        blinkFlag = 1;
        setFrequency(Frequency);
        //EEPROM_write_int(0, Frequency);
        EEPROM_write(0, (unsigned char)(Frequency & 0xFF));
        EEPROM_write(1, (unsigned char)((Frequency >> 8) & 0xFF));
    }
    

    
}
