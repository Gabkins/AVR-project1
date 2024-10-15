#include "data.h"
#include "usart.h"
#include "eeprom.h"

#define F_CPUd1024 10800

extern unsigned short Frequency = 10;
extern bool blinkFlag = false;


void setFrequency(unsigned short freq)
{
  unsigned long temp = F_CPUd1024;  
  unsigned int ocr_value = (unsigned long)(((temp * 10)+ freq/2 ) / freq  - 1);
  
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
