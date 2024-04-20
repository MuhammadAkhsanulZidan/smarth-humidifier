#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "timer.h"

// initialization of associated LED pin as output
void initHumidifierPin(){
  DDRL |= (1<<DDL5);
}

void triggerHumidifier(int delay){

}