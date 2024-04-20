#include <avr/io.h>
#include <util/delay.h>
#include "led.h"

// initialization of associated LED pin as output
void initLED(){
  DDRA |= (1<<DDA0) | (1<<DDA1) | (1<<DDA2);
}

void turnOnLED(unsigned int mode){
  if(mode == 0){
    PORTA |= (1<<PA0); 
    PORTA &= ~((1<<PA1)|(1<<PA2));
  }
  else if(mode == 1){
    PORTA |= (1<<PA1); 
    PORTA &= ~((1<<PA0)|(1<<PA2));
  }
  else {
    PORTA |= (1<<PA2); 
    PORTA &= ~((1<<PA0)|(1<<PA1));     
  }
}
