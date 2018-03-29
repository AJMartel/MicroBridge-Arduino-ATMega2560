#include "LiquidCrystal.h"

#include <stdio.h>
//#include <string.h>
#include <inttypes.h>
//#include "Arduino.h"
#include	<util/delay.h>
#include	<avr/io.h>

uint8_t g_displayfunction;
uint8_t g_displaycontrol;
uint8_t g_displaymode;

uint8_t g_initialized;

uint8_t g_numlines;
uint8_t g_row_offsets[4];


void begin(uint8_t cols, uint8_t lines);
void setRowOffsets(int row0, int row1, int row2, int row3);
void command(uint8_t value);

void send(uint8_t value, uint8_t mode);
void pulseEnable(void);
void write4bits(uint8_t value);
void write8bits(uint8_t value);
// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

void LiquidCrystalinit(void)
{
  DDRA = DDRA | 1; //switch on power pin to periferals card
  PORTA = PORTA | 1;
  DDRL = DDRL | 0x10; //switch on power pin backlight
  PORTL = PORTL & 0xEF;
  _delay_us(500);

  DDRB = DDRB | 0x80; //switch on power pin backlight
  PORTB = PORTB | 0x80;


  //pinMode(rs_pin, OUTPUT);
  DDRK = DDRK | 0x40; //rs pin set as output
  //PORTK = PORTK | 0x20;

  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  /*if (rw_pin != 255) {
    pinMode(rw_pin, OUTPUT);
  }*/

  //pinMode(enable_pin, OUTPUT);
    DDRK = DDRK | 0x80; //enable_pin set as output

  if (fourbitmode)
    g_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else
    g_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

  begin(20, 4);
}

void begin(uint8_t cols, uint8_t lines) {
  if (lines > 1) {
    g_displayfunction |= LCD_2LINE;
  }
  g_numlines = lines;

  setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

  // for some 1 line displays you can select a 10 pixel high font
  //if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
    //g_displayfunction |= LCD_5x10DOTS;
  //}

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  _delay_us(50000);
  // Now we pull both RS and R/W low to begin commands
  //digitalWrite(rs_pin, LOW);
  rs_low;
  enable_low;
  //digitalWrite(enable_pin, LOW);
  /*if (rw_pin != 255) {
    digitalWrite(rw_pin, LOW);
  }*/

  //put the LCD into 4 bit or 8 bit mode
  if (! (g_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms

    // third go!
    write4bits(0x03);
    _delay_us(150);

    // finally, set to 4-bit interface
    write4bits(0x02);
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | g_displayfunction);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | g_displayfunction);
    _delay_us(150);

    // third go
    command(LCD_FUNCTIONSET | g_displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | g_displayfunction);

  // turn the display on with no cursor or blinking default
  g_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  LiquidCrystaldisplay();

  // clear it off
  LiquidCrystalclear();

  // Initialize to default text direction (for romance languages)
  g_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | g_displaymode);

}

/*
   in some 16x4 LCD when line 3 and 4 are not placed correctly you may try:
     setRowOffsets(0x00, 0x40, 0x14, 0x54)
   or
     setRowOffsets(0x00, 0x40, 0x10, 0x50)
 */
void setRowOffsets(int row0, int row1, int row2, int row3)
{
  g_row_offsets[0] = row0;
  g_row_offsets[1] = row1;
  g_row_offsets[2] = row2;
  g_row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LiquidCrystalclear(void)
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void LiquidCrystalhome(void)
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void LiquidCrystalsetCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(g_row_offsets) / sizeof(*g_row_offsets);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= g_numlines ) {
    row = g_numlines - 1;    // we count rows starting w/0
  }

  command(LCD_SETDDRAMADDR | (col + g_row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystaldisplay(void) {
  g_displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | g_displaycontrol);
}
/*
void LiquidCrystalnoDisplay(void) {
  g_displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | g_displaycontrol);
}


// Turns the underline cursor on/off
void LiquidCrystalnoCursor(void) {
  g_displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | g_displaycontrol);
}
void LiquidCrystalcursor(void) {
  g_displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | g_displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystalnoBlink(void) {
  g_displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | g_displaycontrol);
}
void LiquidCrystalblink(void) {
  g_displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | g_displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystalscrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystalscrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystalleftToRight(void) {
  g_displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | g_displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystalrightToLeft(void) {
  g_displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | g_displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystalautoscroll(void) {
  g_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | g_displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystalnoAutoscroll(void) {
  g_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | g_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
/*
void LiquidCrystalcreateChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    LCDwrite(charmap[i]);
  }
}*/

/*********** mid level commands, for sending data/cmds */

void command(uint8_t value) {
  send(value, LOW);
}

uint8_t LCDwrite(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

void LCDprint(const char *str,uint8_t len){
  for (int i=0;i<len;i++){
    LCDwrite(str[i]);
  }
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void send(uint8_t value, uint8_t mode) {
  //digitalWrite(rs_pin, mode);
  if (mode){
    rs_high;
  }
  else{
    rs_low;
  }

  // if there is a RW pin indicated, set it low to Write
  /*if (rw_pin != 255) {
    digitalWrite(rw_pin, LOW);
  }*/

  //if (g_displayfunction & LCD_8BITMODE) {
    //write8bits(value);
  //} else {
    write4bits(value>>4);
    write4bits(value);
  //}
}

void pulseEnable(void) {
  //digitalWrite(enable_pin, LOW);
  enable_low;
  _delay_us(1);
  //digitalWrite(enable_pin, HIGH);
  enable_high;
  _delay_us(1);    // enable pulse must be >450ns
  //digitalWrite(enable_pin, LOW);
  enable_low;
  _delay_us(100);   // commands need > 37us to settle
}

void write4bits(uint8_t value) {
  DDRL = DDRL | 0xCC; //set d0, d1,d2 and d3 to output
  if ((value >> 0) & 0x01){
    d0_high;
  }
  else{
    d0_low;
  }
  if ((value >> 1) & 0x01){
    d1_high;
  }
  else{
    d1_low;
  }
  if ((value >> 2) & 0x01){
    d2_high;
  }
  else{
    d2_low;
  }
  if ((value >> 3) & 0x01){
    d3_high;
  }
  else{
    d3_low;
  }

  pulseEnable();
}

/*
void write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    //pinMode(data_pins[i], OUTPUT);
    //digitalWrite(data_pins[i], (value >> i) & 0x01);
  }

  //pulseEnable();
}*/
