
#include <ioavr.h>
#include <intrinsics.h>
#include <ina90.h>
#include <stdio.h>

#include "usart.h"
#include "crc.h"
#include "eeprom.h"
#include "data.h"
#include "protocol.h"

//GENERAL DEFINES
#define UCHAR unsigned char
#define UINT unsigned int

#define F_CPU 11059200
#define F_CPUd1024 10800

#define DELAY_MS(ms) 	__delay_cycles((F_CPU / 1000) * (ms))

//USART DEFINES
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#define RXD0 PORTE_PORTE0
#define TXD0 PORTE_PORTE1
#define TR PORTE_PORTE2
#define RXC0 7
bool timeout_flag = 0;
UCHAR uart_cnt = 0;
bool start_flag = 1;

//UI DEFINES
#define DATA_PIN PORTA_PORTA6
#define CLK_PIN PORTA_PORTA7

#define SEL1 PORTE_PORTE3   //A0
#define SEL2 PORTE_PORTE4   //A1
#define SEL3 PORTE_PORTE5   //A2

//BUTTONS DEFINES
#define BUT1 PIND_PIND7
#define BUT2 PIND_PIND6
#define BUT3 PIND_PIND5
#define BUT4 PIND_PIND4

  UINT B1_time = 0;
  UINT B2_time = 0;
  UINT B3_time = 0;
  UINT B4_time = 0;
  bool LongFlag = false;
  bool extraLongFlag = false;
  
  
  unsigned short blinkTimer = 0;
  UCHAR blinkCount = 0;

  unsigned short longPressRepeatTimer = 0;
  
  bool T1Flag = 0;

  UCHAR Tickcount = 0;
  

  UINT Timer01_Count = 0;
  UCHAR Timer02_Count = 0;

  UCHAR Position = 1;
  UCHAR d;
  UCHAR light = 10;
  
  UINT Tcount = 0;

 
void shiftOut(UCHAR data)
{
  for(int i = 0; i < 8; i++)
  {
    if (data &(1 << i))
    {
      DATA_PIN = 1;
    }
    else {DATA_PIN = 0;}
    CLK_PIN = 0;
    CLK_PIN = 1;
  }
}

void displayDigit(UCHAR digit, UCHAR pos) {
    UCHAR segment_data;
    
    SEL1 = 1; SEL2 = 1; SEL3 = 1;// Clear selection pins
    
    switch (digit) {
        case 0: segment_data = 0x03; break; //0
        case 1: segment_data = 0X9F; break; //1
        case 2: segment_data = 0X25; break; //2
        case 3: segment_data = 0X0D; break; //3
        case 4: segment_data = 0X99; break; //4
        case 5: segment_data = 0X49; break; 
        case 6: segment_data = 0X41; break; 
        case 7: segment_data = 0X1F; break; 
        case 8: segment_data = 0X01; break; 
        case 9: segment_data = 0X09; break;
        case 10: segment_data = 0xFE; break; //0 FE
        case 11: segment_data = 0XFD; break; //1 FD
        case 12: segment_data = 0XFB; break; //2
        case 13: segment_data = 0XF7; break; //3
        case 14: segment_data = 0XEF; break; //4
        case 15: segment_data = 0XDF; break; //5
        case 16: segment_data = 0XBF; break; //6
        case 17: segment_data = 0X7F; break; //7
        case 100: segment_data = 0XFF; break; //7
    }
    if (pos == 3){segment_data -= 1;}
    
    SEL1 = 1;
    SEL2 = 1; 
    SEL3 = 1;// Clear selection pins
    shiftOut(segment_data);
    

    
    switch (pos) {
        case 1: SEL1 = 0; SEL2 = 0; SEL3 = 0; break; // Select digit 1
        case 2: SEL1 = 1; SEL2 = 0; SEL3 = 0; break; // Select digit 2
        case 3: SEL1 = 0; SEL2 = 1; SEL3 = 0; break; // Select digit 3
        case 4: SEL1 = 1; SEL2 = 1; SEL3 = 0; break; // Select digit 4
        case 5: SEL1 = 0; SEL2 = 0; SEL3 = 1; break; // Select lights
    }
}

void timerSetup()
{
  TCCR0_WGM01 = 1;
  TCCR0_CS02 = 1;    //
  TCCR0_CS01 = 0;
  TCCR0_CS00 = 0;   // div 1024
  
  OCR0 = 64; //64
  TIMSK_OCIE0 = 1;
  
   _SEI(); //toggle break
}

void Timer1_setup()
{
  TCCR1B_WGM12 = 1;
  TCCR1B_CS12 = 1;
  TCCR1B_CS10 = 1;
  TIMSK_OCIE1A = 1;
}

void portSetup()
{
  DDRD = 0; //port input
  PORTD = 0xF0;
  
  DDRA_DDA5 = 1; //1-4 ind
  DDRA_DDA6 = 1; //OUTPUT
  DDRA_DDA7 = 1; //RG

  DDRE_DDE0 = 0; // RXD input
  DDRE_DDE1 = 1; // TXD output  
  DDRE_DDE2 = 1;  //TR output
  DDRE_DDE3 = 1; //seg ports
  DDRE_DDE4 = 1;
  DDRE_DDE5 = 1;
    
  PORTE = 0x01; // RXD pull up
}

void buttonAction(signed char frequencyStep)
{
    if (Frequency < 11 && frequencyStep < 0)
    {frequencyStep = -1;}
    Frequency += frequencyStep;
    
    if (Frequency > 2000) {
        
      Frequency = 2000;
    
    } else if (Frequency < 1) {
        
      Frequency = 1;
    
    }
    setFrequency(Frequency);
}


void blinkDiod(){
  blinkTimer++;
  if(blinkTimer >= 500){
    blinkTimer = 0;
    blinkCount++;
    PORTA_PORTA5 ^= 1;
    if (blinkCount >= 6){
      blinkCount = 0;
      blinkFlag = false;
    }
  }
}


#pragma vector=TIMER1_COMPA_vect   //INTERRUPT
__interrupt void ISR_TickTimer1(void)
{
    if (Frequency == 1)
  {
    Tickcount++;
    if (Tickcount == 2)
    {
      Tickcount = 0;
      light = light == 17 ? 10 : light + 1;
      //T1Flag = 0;
    } 
  } 
  else 
  {
    light = light == 17 ? 10 : light + 1;
  }
}

#pragma vector=TIMER0_COMP_vect   //INTERRUPT
__interrupt void ISR_TickTimer(void)
{
   
  _NOP(); 
  
  Tcount++;

  switch(Position)  {
    case 1:
      d = Frequency / 1000;
      break;
    case 2:
      d = Frequency / 100 % 10;
      break;
    case 3:
      d = Frequency / 10 % 10;
      break;
    case 4:
      d = Frequency % 10;
      break;
    case 5:
      d = light;
      break;
  }

  displayDigit(d, Position);
  if (Frequency < 100) {
    Position = Position == 5 ? 3 : Position + 1;
  } else if (Frequency < 1000) {
    Position = Position == 5 ? 2 : Position + 1;
  } else {
    Position = Position == 5 ? 1 : Position + 1;
  }
  
  
  
  if (!BUT1) {
    B1_time++;
        
    if (B1_time >= 10000) {
      if (!extraLongFlag) {
        extraLongFlag = 1;
        LongFlag = 0;        
      }
      if (Tcount % 100 == 0){
      buttonAction(10);
      }
    } else if (B1_time >= 1000) {
      if (!LongFlag) {
        LongFlag = 1;
      }
      if (Tcount % 100 == 0){
      buttonAction(1);
      }
    }
  } else {
    if (B1_time >= 30 && B1_time < 1000) {
      buttonAction(1);
    }
    B1_time = 0;
    LongFlag = 0;
    extraLongFlag = 0;
  }
  
  
  if (!BUT2) {
    B2_time++;
        
    if (B2_time >= 10000) {
      if (!extraLongFlag) {
        extraLongFlag = 1;
        LongFlag = 0;        
      }
      if (Tcount % 100 == 0){
      buttonAction(-10);
      }
    } else if (B2_time >= 1000) {
      if (!LongFlag) {
        LongFlag = 1;
      }
      if (Tcount % 100 == 0){
      buttonAction(-1);
      }
    }
  } else {
    if (B2_time >= 30 && B2_time < 1000) {
      buttonAction(-1);
    }
    B2_time = 0;
    LongFlag = 0;
    extraLongFlag = 0;
  }
  
  
  if (!BUT3)  {
    B3_time++;
   
  } else {
    if (B3_time >= 30 && B3_time < 1000){
      EEPROM_write_int(0, Frequency);
      blinkFlag = true;
    }
    B3_time = 0;
  }
  
    if (!BUT4)  {
    B4_time++;
   
  } else {
    if (B4_time >= 30 && B4_time < 1000){
      Frequency = EEPROM_read_int(0);
      setFrequency(Frequency);
    }
    B4_time = 0;
  }
  
  if (blinkFlag){
    blinkDiod();
  }
    
}


int main()
{
  Frequency = EEPROM_read_int(0);

  if (Frequency > 2000)
  {
    Frequency = 10;
  } 
  
  setFrequency(Frequency);
  
  portSetup();
  timerSetup();
  Timer1_setup();
  usart_setup(MYUBRR);
  
  while(1)
  {
    TR = 0;
    receivePacket();
    if (sendFlag)
    {
      unsigned char Spacket[5];
      Spacket[0] = Frequency / 1000 + '0';  //  ??????? ????
   
      Spacket[1] = Frequency / 100 % 10 + '0';
      Spacket[2] = Frequency / 10 % 10 + '0';
      Spacket[3] = Frequency % 10 + '0';
      Spacket[4] = ' ';
      int i = 0;
      TR = 1;
      while (i <= 5)
      {      
        USART_Transmit(Spacket[i]);
        i++; 
      }
      USART_Transmit('\0');
      sendFlag = false;
    }
    
  } 
}





