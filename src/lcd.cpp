#include "lcd.h"
#include "Wire.h"
#include "timer.h"

void I2C_LCD::begin(bool beginWire)
{
    if(beginWire){
        Wire.begin();
    }
    I2C_Write(0b00000000); // clear i2c driver
    delayUs(50);
    InitLCD();
}

void I2C_LCD::backlight() {
    _output.Led = 1;
    
    I2C_Write(0b00000000 | _output.Led << 3);
}

void I2C_LCD::backlightOff() {
    _output.Led = 0;
    
    I2C_Write(0b00000000 | _output.Led << 3); 
}

void I2C_LCD::clear() {
    _output.rs = 0;
    _output.rw = 0;

    LCD_Write({0b00000001});
    delayUs(1550);
}

void I2C_LCD::home() {
    _output.rs = 0;
    _output.rw = 0;

    LCD_Write({0b00000010});
    delayUs(1550);
}

void I2C_LCD::leftToRight() {
    _output.rs = 0;
    _output.rw = 0;

    _entryState |= (1 << 1);

    LCD_Write(0b00000100 | _entryState);
    delayUs(37);
}

void I2C_LCD::rightToLeft() {
    _output.rs = 0;
    _output.rw = 0;

    _entryState &= ~(1 << 1);

    LCD_Write(0b00000100 | _entryState);
    delayUs(37);
}

void I2C_LCD::autoscroll() {
    _output.rs = 0;
    _output.rw = 0;

    _entryState |= 1;

    LCD_Write(0b00000100 | _entryState);
    delayUs(37);
}

void I2C_LCD::autoscrollOff() {
    _output.rs = 0;
    _output.rw = 0;

    _entryState &= ~1;

    LCD_Write(0b00000100 | _entryState);
    delayUs(37);
}

void I2C_LCD::display() {
    _output.rs = 0;
    _output.rw = 0;

    _displayState |= (1 << 2);

    LCD_Write(0b00001000 | _displayState);
    delayUs(37);
}

void I2C_LCD::displayOff() {
    _output.rs = 0;
    _output.rw = 0;

    _displayState &= ~(1 << 2);

    LCD_Write(0b00001000 | _displayState);
    delayUs(37);
}

void I2C_LCD::cursor() {
    _output.rs = 0;
    _output.rw = 0;

    _displayState |= (1 << 1);

    LCD_Write(0b00001000 | _displayState);
    delayUs(37);
}

void I2C_LCD::cursorOff() {
    _output.rs = 0;
    _output.rw = 0;

    _displayState &= ~(1 << 1);

    LCD_Write(0b00001000 | _displayState);
    delayUs(37);
}

// void I2C_LCD::blink() {
//     _output.rs = 0;
//     _output.rw = 0;

//     _displayState |= 1;

//     LCD_Write(0b00001000 | _displayState);
//     delayUs(37);
// }

// void I2C_LCD::blinkOff() {
//     _output.rs = 0;
//     _output.rw = 0;

//     _displayState &= ~1;

//     LCD_Write(0b00001000 | _displayState);
//     delayUs(37);
// }

void I2C_LCD::scrollDisplayLeft() {
    _output.rs = 0;
    _output.rw = 0;

    LCD_Write(0b00011000);
    delayUs(37);
}

void I2C_LCD::scrollDisplayRight() {
    _output.rs = 0;
    _output.rw = 0;

    LCD_Write(0b00011100);
    delayUs(37);
}

void I2C_LCD::createChar(uint8_t memory_location, uint8_t charmap[]) {
    _output.rs = 0;
    _output.rw = 0;

    memory_location %= 8;

    LCD_Write(0b01000000 | (memory_location << 3));
    delayUs(37);

    for (int i = 0; i < 8; i++)
        write(charmap[i]);

    setCursor(0, 0); // Set the address pointer back to the DDRAM
}

void I2C_LCD::setCursor(uint8_t col, uint8_t row) {
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    _output.rs = 0;
    _output.rw = 0;
     // sanity limits
    if (col > _columnMax) { col = _columnMax; }
    // sanity limits
    if (row > _rowMax) { row = _rowMax; }

    uint8_t newAddress = row_offsets[row] + col;

    LCD_Write(0b10000000 | newAddress);
    delayUs(37);
}

size_t I2C_LCD::write(uint8_t character) {
    _output.rs = 1;
    _output.rw = 0;

    LCD_Write(character);
    delayUs(41);

    return 1;
}

void I2C_LCD::I2C_Write(uint8_t output){
    Wire.beginTransmission(_address);
    Wire.write(output);
    Wire.endTransmission();
}



void I2C_LCD::LCD_Write(uint8_t output, bool fourBit) {
    _output.data = output;

    _output.E = 1;
    I2C_Write(_output.getUpperNibble());

    delayUs(1);

    _output.E = 0;
    I2C_Write(_output.getUpperNibble());

    if (!fourBit) {
        delayUs(37);

        _output.E = 1; //Enable high
        I2C_Write(_output.getLowNibble());
        delayUs(1); 

        _output.E = 0; //Enable low
        I2C_Write(_output.getLowNibble());
    }
}


void I2C_LCD::InitLCD() {
    _output.rs = 0;
    _output.rw = 0;

    delay(50);

    LCD_Write(0b00110000, true);
    delayUs(4200);

    LCD_Write(0b00110000, true);
    delayUs(150);

    LCD_Write(0b00110000, true);
    delayUs(37);
    
    LCD_Write(0b00100000, true);
    delayUs(37);

    LCD_Write(0b00101000);
    delayUs(37);

    displayOff();
    clear();
    leftToRight();
}

