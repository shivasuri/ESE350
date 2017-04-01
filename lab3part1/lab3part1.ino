#include < avr / interrupt.h > 
#include < avr / io.h > 
#include < stdio.h > 
#include "uart.h"
#include < string.h >

#define BUTTON PB0# define LED PB1

uint32_t numOverflows;
uint16_t timeButtonPressed;
uint16_t timeButtonReleased;
uint8_t buttonWasPressed;
uint8_t waitForSpace;

char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
char * morse[] = { /* an array of pointers to char */
  ".-", "-...", "-.-.", "-..", ".",                 /* a-e */
  "..-.",  "--.",  "....",  "..",  ".---",          /* f-j */
  "-.-",  ".-..",  "--",  "-.","---",               /* k-o */
  ".--.",  "--.-",".-.",  "...","-",                /* p-t */
  "..-",  "...-",  ".--",  "-..-",  "-.--",         /* u-y */
  "--..",                                           /* z   */
  "-----",  ".----",  "..---",  "...--",  "....-",  /* 0-4 */
  ".....",  "-....",  "--...","---..",  "----.",    /* 5-9 */
  ".......",  "...-.-" /* space, end of transmission*/
};
char morse_buffer[10] = "";

// unsigned int diff, edge1, edge2, overflows;
// unsigned long pulse_width;

ISR(TIMER1_OVF_vect) {
  numOverflows++;
}

// int main() {
//     uart_init();

// connect led to pin 13
//   DDRB = 0b111110; //sets pins 9-13 as outputs and pin 8 as input

//   TCCR1B |= 0x03; // start timer, prescaler = 64
//   TCCR1B |= 0x40; // capture rising edges
//   overflows = 0;
//   TIMSK1 |= 0x01; // enable overflow interrupt
//   TIFR1 |= 0x20; // clear input capture flag
//   if ((PINB & 0x01) == 1) {
//     printf("true");
//     PORTB &= (1 << PB5);
//   }
//   printf("here1");
//   while (!(TIFR1 & 0x20)); // wait until an edge
//   printf("here2");
//   sei(); // enable interrupts
//   edge1 = ICR1; // save time of first edge
//   TCCR1B &= 0xBF; // capture falling edges
//   TIFR1 |= 0x20; // clear input capture flag
//   while(!(TIFR1 & 0x20)); //wait until falling edge
//   cli(); // disable interrupts
//   edge2 = ICR1; // save time of the second edge
//   if (edge2 < edge1) {
//     overflows--;
//   }
//   diff = edge2 - edge1;
//   pulse_width = (long)overflows * 65536u +
//   (long)diff;
//   printf("pulse width = 4us x %u\n", pulse_width);
//   for(;;);
// }

ISR(TIMER1_CAPT_vect) {
  //printf('hi');
  // int button = PINB & (1 << PB0); //check if button is pressed
  // for (int i = 0; i < 100; i++) {

  //   int newButton = PINB & (1 << PB0); 

  //     if (button != newButton) {
  //   return; 
  //   }

  // }

  // if (PINB & (1 << PB0)) { 
  //   PORTB &= ~(1 << PB5); //PB5: low
  // }
  // else {
  //   PORTB |= (1 << PB5);
  // }
  PORTB ^= (1 << PB5); //turn on/off output
  TCCR1B ^= (1 << ICES1); //0: falling edge 1: rising edge (switch between)
  //TIFR1 |= (1 << ICF1); //clearFlag
  TIFR1 |= 0x20;

  cli(); // disable interrupts
  unsigned char numCycles = 100;
  unsigned char buttonPressed = !(PINB & (1 << BUTTON));
  for (int i = 0; i < numCycles; i++) {
    if (buttonPressed != !(PINB & (1 << BUTTON))) {
      return;
    }
  }

  if (!buttonPressed) { // button released
    TCCR1B &= ~(1 << ICES1); // detect falling edges
    PORTB &= ~(1 << LED); // turn LED off

    timeButtonReleased = TCNT1;
    buttonWasPressed = 1;
  } else { // button pressed
    TCCR1B |= (1 << ICES1); // detect rising edges
    PORTB |= (1 << LED); // turn LED on

    numOverflows = 0;
    timeButtonPressed = TCNT1;
    waitForSpace = 0; // stop waiting for spaces
  }

  sei(); // enable interrupts
}
void initState() {
  numOverflows = 0;
  timeButtonPressed = 0;
  timeButtonReleased = 0;
  buttonWasPressed = 0;
  waitForSpace = 0;
}

int main() {
  uart_init();
  initState();

  DDRB |= (1 << PB5); //output PB5 (pin 13)
  DDRB &= ~(1 << PB0); //input PB0 (pin 8)
  TCCR1B |= (1 << CS10); //timer w/o prescaler 
  TCCR1B &= ~(1 << ICES1); //set to detect falling edge
  TIMSK1 |= (1 << ICIE1); //input capture interrupt enable 
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt

  // TCNT0 = 0; //begin timer
  // //TIFR1 |= (1 << ICF1); //clearflag
  // TIFR1 |= 0x20;

  // DDRB |= (1 << PB5); //output PB5 (pin 13)
  // PORTB |= (1 << PB5);
  sei();
  while (1) {
    if (buttonWasPressed) {
      cli();
      buttonWasPressed = 0;
      uint16_t deltaT = timeButtonReleased - timeButtonPressed;
      uint32_t duration;
      if (deltaT < 0) {
        duration = 65535 * (numOverflows + 1) + deltaT;
      } else {
        duration = 65535 * numOverflows + deltaT;
      }
      //printf("Duration of button press (ticks): %ld\n", duration);
      if (duration < 480000) {
        // do nothing
      } else if (duration >= 480000 && duration < 3200000) {
        // putchar('.');
        strcat(morse_buffer, ".");
        PORTB |= (1 << 2); // Light up the dot
        PORTB &= ~(1 << 3);
        if (strlen(morse_buffer) > 7) { // clear the string if length > 7
          memset(morse_buffer, 0, 10);
        }
      } else {
        // putchar('-');
        strcat(morse_buffer, "-");
        PORTB |= (1 << 3); // light up the dash
        PORTB &= ~(1 << 2);
        if (strlen(morse_buffer) > 7) { // clear buffer
          memset(morse_buffer, 0, 10);
        }
      }
      waitForSpace = 1;
      numOverflows = 0;
      sei();
    }

    if (waitForSpace) {
      uint16_t currentTime = TCNT1;
      uint16_t deltaT = currentTime - timeButtonReleased;
      uint32_t duration;
      if (deltaT < 0) {
        duration = 65535 * (numOverflows + 1) + deltaT;
      } else {
        duration = 65535 * numOverflows + deltaT;
      }
      if (duration >= 6400000) {
        waitForSpace = 0;
        // putchar(' ');
        PORTB &= ~(1 << 2); // unlight the dash and dot
        PORTB &= ~(1 << 3);
        int i;
        for (i = 0; i < 37; i++) {
          if (strcmp(morse[i], morse_buffer) == 0) {
            putchar(alpha[i]);
          }
        }
        free i;
        if (strlen(morse_buffer) > 7) { // clear buffer
          memset(morse_buffer, 0, 10);
        }
      }
    }
  }
}
