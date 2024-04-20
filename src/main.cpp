#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "switch.h"
#include "timer.h"
#include "Keypad.h"
#include "lcd.h"
#include "humidifier.h"
#include "adc.h"

// state machines for push button
typedef enum stateButton_enum
{
  released,
  debounce_press,
  pressed,
  debounce_release
} stateButton;

// state machines for mode
typedef enum stateMode_enum
{
  normal,
  autotemp,
  autosched,
} stateMode;

volatile stateButton button_state = released;
volatile stateMode mode_state = normal;
int change = 0;
int schedIndex = 0;
int temptrIndex = 0;

//temperature and schedule value for which the humidifier is set to turn on
char temptrToOnStr[2] = "";
char schedToOnStr[3] = "";
int temptrToOnVal = 0;
int schedToOnVal = 0;

//read sensor value
float reading;

//I2C LCD initialization
I2C_LCD lcd(0x27, 16, 2);

//Check if value has been inputed for schedule or temperature
bool schedEnter = false;
bool temptrEnter = false;

//keypad initialization
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {30, 31, 32, 33}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {34, 35, 36, 37}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//temperature sampling, more consistent the value, more confidence level
int temptrConfidence = 0;

//start time for schedule
unsigned long startTime;

int ddelay=0;

void delayPotentiometer2() {
  uint16_t adcValue = readADC();  // Read the ADC value (0 - 1023)

  unsigned int mdelay;
  if (adcValue < 205) mdelay = 2;       // 0 - 1k ohm
  else if (adcValue < 410) mdelay = 4;  // 1k - 2k ohm
  else if (adcValue < 615) mdelay = 6;  // 2k - 3k ohm
  else if (adcValue < 820) mdelay = 8;  // 3k - 4k ohm
  else mdelay = 10;                     // 4k - 5k ohm

  ddelay=mdelay;
}

//array of chars to integer conversion
int convertToInt(char *str)
{
  int result = 0;
  int i = 0;

  while (isdigit(str[i]))
  {
    result = result * 10 + (str[i] - '0');
    i++;
  }

  return result;
}

void setup()
{
  Serial.begin(9600);
  initLED();
  init();
  lcd.begin();
  lcd.display();
  lcd.backlight();
  initTimer1();
  initSwitchPB3();
  initHumidifierPin();
  initADC0();
  
  DDRF &= ~(1 << DDF1);
}

void loop()
{
  
  // delayPotentiometer2();
  if (change == 0)
  {
    switch (mode_state)
    {
    case normal:
      turnOnLED(0);
      lcd.clear();
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("normal");
      // temptrConfidence = 0;
      break;
    case autotemp:
      turnOnLED(1);
      lcd.clear();
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("auto-temp");
      lcd.setCursor(0, 1);
      lcd.print("On at(`C):");
      break;
    case autosched:
      turnOnLED(2);
      lcd.clear();
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("auto-sched");
      lcd.setCursor(0, 1);
      lcd.print("On after:");
      break;
    }
    change = 1;
  }

  if (mode_state == autotemp)
  {
    char key = keypad.getKey();
    if (key)
    {
      // time format:  _ _ : _ _
      if (key != '#' && key != '*' && key != 'D' && key != 'C' && key != 'B' && key != 'A')
      {
        if (temptrEnter)
        {
          lcd.setCursor(0, 0);
          lcd.print("auto-temp");
          lcd.setCursor(0, 1);
          lcd.print("On at(`C):");
          temptrConfidence = 0;
        }
        lcd.setCursor(temptrIndex + 10, 1);
        temptrToOnStr[temptrIndex] = key;
        lcd.print(temptrToOnStr[temptrIndex]);
        temptrIndex++;
        if (temptrIndex > 1)
        {
          temptrIndex = 0;
        }
      }
      else if (key == '#')
      {
        temptrEnter = true;
        temptrToOnVal = convertToInt(temptrToOnStr);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("on at: ");
        lcd.print(temptrToOnVal);
        lcd.print("`C");
      }
    }

    if (temptrEnter)
    {
      int rawValue = analogRead(A1);
      reading = ((5.0 * rawValue * 100.0) / 1024);
      lcd.setCursor(0, 1);
      lcd.print("temperature: ");
      lcd.print(reading);
      _delay_ms(1000);
      if (reading > temptrToOnVal)
      {
        temptrConfidence++;
      }
      else if (reading < temptrToOnVal)
      {
        temptrConfidence--;
      }
      if (temptrConfidence > 1)
      {
        delayPotentiometer2();
         PORTL &= ~((1<<PL5));  
  _delay_ms(5000);
  PORTL |= (1<<PL5);
  _delay_ms(200);
  PORTL &= ~((1<<PL5));  
  _delay_ms(5000);
  PORTL |= (1<<PL5);
  _delay_ms(200);
       temptrConfidence = 0;
      }
    }
  }

  else if (mode_state == autosched)
  {
    char key = keypad.getKey();

    if (key)
    {
      // time format:  _ _ : _ _
      if (key != '#' && key != '*' && key != 'D' && key != 'C' && key != 'B' && key != 'A')
      {
        if (schedEnter)
        {
          lcd.setCursor(0, 0);
          lcd.print("auto-sched");
          lcd.setCursor(0, 1);
          lcd.print("On after:");
          schedToOnStr[3] = "";
          schedIndex = 0;
          schedEnter = false;
        }
        lcd.setCursor(schedIndex + 10, 1);
        schedToOnStr[schedIndex] = key;
        lcd.print(schedToOnStr[schedIndex]);
        schedIndex++;
        if (schedIndex > 2)
        {
          schedIndex = 0;
        }
      }
      else if (key == '#')
      {
        schedEnter = true;
        schedToOnVal = convertToInt(schedToOnStr);
        startTime = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("auto on after: ");
        lcd.print(schedToOnVal);
        lcd.print(" min");
      }
    }

    if (schedEnter && (millis()-5000 - startTime > schedToOnVal * 1000))
    {
               PORTL &= ~((1<<PL5));  
  _delay_ms(5000);
  PORTL |= (1<<PL5);
  _delay_ms(200);
  PORTL &= ~((1<<PL5));  
  _delay_ms(5000);
  PORTL |= (1<<PL5);
  _delay_ms(200);
      startTime = millis();
    }
  }

  switch (button_state)
  {
  case released:
    break;

  case debounce_press:
    delayUs(1000);
    button_state = pressed;
    break;

  case pressed:
    break;

  case debounce_release:
    delayUs(1000);
    button_state = released;
    break;
  }
}

ISR(PCINT0_vect)
{
  /*if interrupt is triggered when button was released,
    change button state to press debouncing.
    process is then continued in push button state machine in main()
  */
  if (button_state == released)
  {
    button_state = debounce_press;
  }

  /*if interrupt is triggered when button was pressed,
    check system mode and change it,
    change button state to release debouncing.
    process is then continued in push button state machine in main()
  */
  else if (button_state == pressed)
  {
    if (mode_state == normal)
    {
      mode_state = autotemp;
      temptrEnter = false;
      schedEnter = false;
      temptrConfidence = 0;
    }

    else if (mode_state == autotemp)
    {
      mode_state = autosched;
      temptrEnter = false;
      schedEnter = false;
    }
    else if (mode_state == autosched)
    {
      mode_state = normal;
      temptrEnter = false;
      schedEnter = false;
    }
    button_state = debounce_release;
    change = 0;
  }
}