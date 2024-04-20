#include <avr/io.h>

#ifndef ADC_H
#define ADC_H


void initADC0();
uint16_t readADC();
int delayPotentiometer();

#endif