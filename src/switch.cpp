#include <avr/io.h>

void initSwitchPB3(){
    DDRB &= ~(1<<DDB3); //regist pin PB3 as input
    PORTB |= (1<<PB3); //pull up resistor on PB3
        
    //Enable group PCINT
    PCICR |= (1 << PCIE0); 

    //Enable local PCINT
    PCMSK0 |= (1 << PCINT3);
}
