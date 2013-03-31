
#include <stdio.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <uart.h>
//#define F_CPU 12000000
#include <util/delay.h>


#define BUTTON0 3
#define BUTTON1 4

#define COL_R 0
#define COL_G 1
#define COL_B 2

#define BIT_DOWN(reg, bit) \
  reg &= ~(1<<bit)

#define BIT_UP(reg, bit) \
  reg |= (1<<bit)

int ADC_get(int num)
{
  int r=0;
  ADCSRA|=(1<<ADIF);
  while ((ADCSRA&(1<<ADIF))){};

  ADMUX &= ~(0xf);
  ADMUX |= num&0x0f;

  ADCSRA |= (1 << ADSC); 

  while (!(ADCSRA&(1<<ADIF))){};
  //conversion is ready
  // reset converstion complete flag
  ADCSRA|=(1<<ADIF);

  // read data, ADCL - first
  r=ADCL;
  r=(ADCH<<8)|r;

  return r;
}

int main(void)
{
  wdt_disable();
  cli();
  //DDRC=0xff;
  //PORTC=0xff;
  DDRD=0xfe;
  //PIND=0xff;
  
  uart_init();
  uart_stdio();


  ///ADC 
  // 000 0x00 -- 2
  // 001 0x01 -- 2
  // 010 0x02 -- 4
  // 011 0x03 -- 8
  // 100 0x04 -- 16
  // 101 0x05 -- 32
  // 110 0x06 -- 64
  // 111 0x07 -- 128
  ADCSRA |=  (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar 
  ADMUX |= (1 << REFS0) | (1 << REFS1); // Set ADC reference to AVCC
  // 0110 0x06 -- ADC6
  // 0111 0x07 -- ADC7
  // 1000 0x08 -- ADC8(temperature)
  // 1110 0x0e -- 1.1v
  // 1111 0x0f -- GND
  ADMUX |= 0x07;

  //ADCSRA |= (1 << ADFR);  // Set ADC to Free-Running Mode
  ADCSRA |= (1 << ADEN);  // Enable ADC
  ADCSRA |= (1 << ADSC);  // Start A2D Conversions

  sei();
  printf("HELLO WOLRD\n");

  long int i=1;
  int ai=0;
  while(1) {
    int a,b,delta, term;

    int aref,gnd;
    a=ADC_get(6);
    b=ADC_get(7)+6;//погрешность
    term=ADC_get(8);
    aref=ADC_get(0x0e);
    gnd=ADC_get(0x0f);
    delta=b-a;

    i++;
  printf("I %li (a:%i,b:%i, delta:%i) term: %i aref: %i, gnd: %i\n", i, a, b, delta, term, aref, gnd);

    //BIT_DOWN(PORTC, 3);
//    BIT_DOWN(PORTD, 6);
//l    _delay_ms(500);
    BIT_UP(PORTD, 6);
 /*
  if(i%2)
  else
    BIT_DOWN(PORTD, 6);
    */
  _delay_ms(500);
    //BIT_UP(PORTC, 3);
   //*/
   //_delay_ms(1000);
  }


  return 0;
}
