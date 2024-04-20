#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

void initTimer1();
void delayUs(unsigned int delay);
void delaySec(unsigned int delay);
void delayMin(unsigned int delay);

#endif
