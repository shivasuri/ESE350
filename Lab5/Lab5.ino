/*
 * Lab5.c
 *
 * Created: 3/11/2017 5:25:16 PM
 * Author : Roger
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile int isRising = 1;
unsigned int edge1, edge2;
unsigned long pulse_width;

ISR (TIMER1_COMPA_vect) {
  Serial.print('s');
  TIMSK1  &= ~((1<<1) | (1<<4));        // disable interrupt
  PORTB   &= ~(1 << 1);    // set PB1 out to 0;
  DDRB    &= ~(1 << 1);    // set PB1 as input
  TCCR1A  &= ~(1 << 6);    // toggle off output compare
  TCCR1B  |= (1 << 6);    // detect rising edges
}

ISR (TIMER1_CAPT_vect) {
  if (isRising == 1) {
    edge1 = ICR1;
    TCCR1B  &= 0xBF;
    isRising = 0;
  } else {
    edge2 = ICR1;
    pulse_width = edge2 - edge1;
    Serial.print(pulse_width);
    isRising = 1;
    TCCR1B  |= (1 << 6);
  }
}

int main(void)
{
  Serial.begin(9600);
  Serial.print('b');
  Serial.print('\n');

  DDRB  |= (1 << 1);
  PORTB |= (1 << 1);
  TCCR1A  |= (1 << 6);    // toggle output compare
  TCCR1B  |= (1 << 1);    // precaler 8

  TIMSK1  |= (1 << 1) | (1<<4);  // enable OCR1A interrupt
  OCR1A += TCNT1 + 10;    // interrupt in 5 us
  sei();
    /* Replace with your application code */
    while (1) 
    {
//      Serial.print("hello");
    }
  return 1;
}


