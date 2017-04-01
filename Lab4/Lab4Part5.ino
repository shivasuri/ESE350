/*
 * Part3.c
 *
 * Created: 2/15/2017 5:25:55 PM
 * Author : Roger
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "uart.h"

volatile int hi_half_period;
volatile int lo_half_period;
volatile float elapsed_time = 0.0;
volatile unsigned int current_row = 2; 
unsigned int foursecperiod = 250;
unsigned int buttonpress = 0;
volatile unsigned int d2;
volatile unsigned int d3;
volatile unsigned int d4;
volatile unsigned int d5;

void switch_hit(int hi, int lo) {
  buttonpress = 1;
//  Serial.print(hi);
//  Serial.print('\n'); 
//  Serial.print(lo);
//  Serial.print('\n');
//
  TCCR0A  |= (1 << 6);
  TCCR0B  |= (1 << 3) | 0x04;
  TCCR1A  |= (1 << 6);
  TCCR1B  |= (1 << 3) | 0x01;
//  TIMSK0  |= (1<<1);
//  TIMSK0  |= (1<<4);
//  TIMSK1  |= (1<<1); 
//  TIMSK1  |= (1<<3);
//  TIMSK1  |= (1<<4);
  hi_half_period = hi;
  lo_half_period = lo / 64;
  OCR1A = hi_half_period;
  OCR0A = lo_half_period;
}


ISR (TIMER2_COMPA_vect) {
  OCR2A += foursecperiod;
  // drive a row low, read columns
  DDRC |= 0x3C;
//  PORTC |= 0x00;
  PORTC |= 0x3C;
  PORTC &= ~(1 << current_row);
//  Serial.print(PORTC);
//  Serial.print('\n');
//  Serial.print(PIND);
//  Serial.print('\n');
  d2 = PIND & 0x04;
  d3 = PIND & 0x08;
  d4 = PIND & 0x10;
  d5 = PIND & 0x20;

//  Serial.print(PIND & 0x04);
//  Serial.print('\n');
//  Serial.print(PIND & 0x08);
//  Serial.print('\n');
//  Serial.print(PIND & 0x10);
//  Serial.print('\n');
//  Serial.print(PIND & 0x20);
//  Serial.print('\n');
//  switch_hit(6617, 11477);

  if (d2 == 0) {
    switch(current_row) {
      case 2 :
        Serial.print('4');
        switch_hit(6617, 11477);
        break;
      case 3 : 
        Serial.print('5');
        switch_hit(6617, 10389);
        break;
      case 4 :
        Serial.print('6');
        switch_hit(6617, 9289);
        break;
      case 5 :
        Serial.print('B');
        switch_hit(6617, 8501);
        break;
      default : break;
    }
  } else if (d3 == 0) {
    switch(current_row) {
      case 2 :
        Serial.print('C');
        switch_hit(5987, 11477);
        break;
      case 3 :
        Serial.print('9');
        switch_hit(5987, 10389);
        break;
      case 4 :
        Serial.print('8');
        switch_hit(5987, 9289);
        break;
      case 5 :
        Serial.print('7');
        switch_hit(5987, 8501);
        break;
      default : break;
    }
   } else if (d4 == 0) {
    switch(current_row) {
      case 2 :
        Serial.print('D');
        switch_hit(5415, 11477);
        break;
      case 3 :
        Serial.print('#');
        switch_hit(5415, 10389);
        break;
      case 4 :
        Serial.print('0');
        switch_hit(5415, 9289);
        break;
      case 5 :
        Serial.print('*');
        switch_hit(5415, 8501);
        break;
      default : break;
    }
  } else if (d5 == 0) {
    switch(current_row) {
      case 2 :
        Serial.print('A'); 
        switch_hit(4898, 11477);
        break;
      case 3 :
        Serial.print('3'); 
        switch_hit(4898, 10389);
        break;
      case 4 :
        Serial.print('2'); 
        switch_hit(4898, 9289);
        break;
      case 5 :
        Serial.print('1');
        switch_hit(4898, 8501);
        break;
      default : break;
    }
  } 
  current_row ++;
  if (current_row == 6) {;
    current_row = 2;
    if (buttonpress == 0) {
      TCCR1A  &= ~0x40;
      TCCR1B  &= ((1 << 3) | 0x01);
      TCCR0A  &= ~0x40;
    }
    buttonpress = 0;
  }
}

void timer2_init() {
  
}

int main()
{
  Serial.begin(9600);
  DDRB  |= 0x0F;
  DDRC  |= 111100;  // set PC2-5 as outputs
  DDRD  |= 0x00;  // set inputs
  
  // set timer 2
  TCCR2A  |= 0x40; // toggle pin on reset
//  TCCR2A  |= 0x02; // CTC mode
  TIMSK2  |= 0x02; // enable output compare A interrupt
  TIMSK2  |= (1 << 4);
  TCCR2B  |= (1 << 2); // prescaler to 256
  OCR2A += foursecperiod;    // interrupt every 4 ms
  sei();
  
    /* Replace with your application code */
    while (1) 
    {
    }
  return 1;
}


