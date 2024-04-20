#include <avr/io.h>

void initADC0()
{
  // Set reference to AVCC and left adjust result
  ADMUX = (1 << REFS0);
  ADMUX &= ~(1<< REFS1);

  // Enable the ADC and set the division factor to 64 (16MHz/64 = 250kHz)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  ADCSRA |= (1<<ADEN);
}

uint16_t readADC()
{
  // Select the appropriate channel (0-7)
  ADMUX &= 0xF0;
  ADMUX |= 0x00;

  // Start the conversion
  ADCSRA |= (1 << ADSC); 
  // Wait for the conversion to finish
  while (ADCSRA & (1 << ADSC));

  // Return the ADC value
  return ADC;
}

int delayPotentiometer() {
  uint16_t adcValue = readADC();  // Read the ADC value (0 - 1023)

  unsigned int delay;
  if (adcValue < 205) delay = 2;       // 0 - 1k ohm
  else if (adcValue < 410) delay = 4;  // 1k - 2k ohm
  else if (adcValue < 615) delay = 6;  // 2k - 3k ohm
  else if (adcValue < 820) delay = 8;  // 3k - 4k ohm
  else delay = 10;                     // 4k - 5k ohm

  return delay;
}