#ifndef LiquidCrystal_h
#define LiquidCrystal_h

#include <inttypes.h>


// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00
//#define rs_pin A14
//#define rw_pin 255
//#define enable_pin A15

#define rs_high PORTK = PORTK | 0x40
#define rs_low PORTK = PORTK & 0xBF

#define enable_high PORTK = PORTK | 0x80
#define enable_low PORTK = PORTK & 0x7F

#define d0_high PORTL = PORTL | 0x04
#define d0_low PORTL = PORTL & 0xFB

#define d1_high PORTL = PORTL | 0x08
#define d1_low PORTL = PORTL & 0xF7

#define d2_high PORTL = PORTL | 0x40
#define d2_low PORTL = PORTL & 0xBF

#define d3_high PORTL = PORTL | 0x80
#define d3_low PORTL = PORTL & 0x7F

//const char data_pins[8]={47,46,43,42,0,0,0,0};

#define fourbitmode  1

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

void LiquidCrystalinit(void);

void LiquidCrystalclear(void);
void LiquidCrystalhome(void);

//void LiquidCrystalnoDisplay(void);
void LiquidCrystaldisplay(void);
/*void LiquidCrystalnoBlink(void);
void LiquidCrystalblink(void);
void LiquidCrystalnoCursor(void);
void LiquidCrystalcursor(void);
void LiquidCrystalscrollDisplayLeft(void);
void LiquidCrystalscrollDisplayRight(void);
void LiquidCrystalleftToRight(void);
void LiquidCrystalrightToLeft(void);
void LiquidCrystalautoscroll(void);
void LiquidCrystalnoAutoscroll(void);

void LiquidCrystalcreateChar(uint8_t, uint8_t[]);
*/
void LiquidCrystalsetCursor(uint8_t, uint8_t);
uint8_t LCDwrite(uint8_t value);
void LCDprint(const char *str,uint8_t len);

#endif
