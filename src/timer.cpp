#include "timer.h"

// Initialize timer 1 with CTC mode
void initTimer1(){
    //TCCR1A, Timer/Counter Control Register 1A, set WGM10 and WGM11 here
    TCCR1A &= ~( (1 << WGM10) | ( 1<< WGM11));
    //TCCR1B, Timer/Counter Control Register 1B, set WGM12 and WGM13 here
    TCCR1B |= ( 1 << WGM12);
    TCCR1B &= ~ ( 1 << WGM13); 
}

/* This delays the program an amount of microseconds specified by unsigned int delay.
*/
void delayUs(unsigned int delay){
  //Using #counts = (td* fCPU)/PS  = (1us*16MHz)/8 -1 = 1
  OCR1A = 1; 

  //Timer/Counter 0 Interrupt Flag Register, this one will use OCF1A
  //Set the flag down
  TIFR1 |= (1 << OCF1A);

  TCNT1 = 0;  //Timer/Counter Register starts from 0

  /*Turn on timer
    using prescaler of 8, Clock Select CS10 = 0, CS11 = 1, CS12 = 0.
    they are set in TCCR1B (Timer/Counter Control Register 1B)
  */
  TCCR1B |= (1 << CS11);
  TCCR1B &= ~((1<<CS12)|(1<<CS10));

  unsigned int count = 0;
  while (count < delay) {
    /*when count is less than delay,
      if the flag is raised (1 us has been elapsed): 
        increment count and down the flag again
        this repeat until 1us*delay has been elapsed
    */
    if ((TIFR1 & (1 << OCF1A))) {
      count++;
      TIFR1 |= (1 << OCF1A);
    } 
  }

  // after exiting the while loop the time elapsed has been 1us*delay
  // turn off the Timer1 clock
  // Set CS00, CS01, and CS02 to 0
    TCCR1B &= ~( ( 1 << CS12) | ( 1 << CS11) | (1 << CS10));
}


void delaySec(unsigned int delay)
{
  // Initialize Timer1 compare match register for 1 second
  OCR1A = (F_CPU / 1024) - 1;

  // Turn on the timer in CTC mode with 1024 prescaler
  TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);

  for (unsigned int i = 0; i < delay; i++)
  {
    // Clear the timer by setting TCNT1 to 0
    TCNT1 = 0;

    // Wait for the compare match flag (OCF1A) to be set
    while (!(TIFR1 & (1 << OCF1A)));

    // Clear the compare match flag (OCF1A)
    TIFR1 |= (1 << OCF1A);
  }

  // Turn off the timer
  TCCR1B &= ~((1 << WGM12) | (1 << CS12) | (1 << CS10));
}

void delayMin(unsigned int delay)
{
  // Initialize Timer1 compare match register for 1 second
  OCR1A = (F_CPU / 1024) - 1;

  // Turn on the timer in CTC mode with 1024 prescaler
  TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);

  for (unsigned long i = 0; i < delay * 60; i++)  // Multiply by 60 to convert minutes to seconds
  {
    // Clear the timer by setting TCNT1 to 0
    TCNT1 = 0;

    // Wait for the compare match flag (OCF1A) to be set
    while (!(TIFR1 & (1 << OCF1A)));

    // Clear the compare match flag (OCF1A)
    TIFR1 |= (1 << OCF1A);
  }

  // Turn off the timer
  TCCR1B &= ~((1 << WGM12) | (1 << CS12) | (1 << CS10));
}