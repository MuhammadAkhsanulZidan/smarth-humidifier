#ifndef LCD_H
#define LCD_H

#include "Arduino.h"

struct OutputState {
    uint8_t rs = 0;
    uint8_t rw = 0;
    uint8_t E = 0;
    uint8_t Led = 0;
    uint8_t data = 0;

    uint8_t getLowNibble() {
        uint8_t nibble = rs;
        nibble |= rw << 1;
        nibble |= E << 2;
        nibble |= Led << 3;
        nibble |= (data & 0x0F) << 4;

        return nibble;
    }

    uint8_t getUpperNibble() {
        uint8_t nibble = rs;
        nibble |= rw << 1;
        nibble |= E << 2;
        nibble |= Led << 3;
        nibble |= (data & 0xF0);
        return nibble;
    }
};

class I2C_LCD : public Print {
    public:
       I2C_LCD(uint8_t address, uint8_t columns = 16, uint8_t rows = 2)
        : _address(address), _columnMax(--columns), _rowMax(--rows) {}

        void begin(bool beginWire = true);
        void backlight();
        void backlightOff();

        void clear();
        void home();
        void leftToRight();
        void rightToLeft();
        void autoscroll();
        void autoscrollOff();
        void display();
        void displayOff();
        void cursor();
        void cursorOff();
        void scrollDisplayLeft();
        void scrollDisplayRight();
        void createChar(uint8_t location, uint8_t charmap[]);
        void setCursor(uint8_t col, uint8_t row);
        
        virtual size_t write(uint8_t character);

    private:
        uint8_t _address;
        uint8_t _columnMax;
        uint8_t _rowMax;
        OutputState _output;
        uint8_t _displayState = 0x00;
        uint8_t _entryState = 0x00;

        void InitLCD();
        void I2C_Write(uint8_t output);
        void LCD_Write(uint8_t output, bool eightbit = false);
};

#endif