/*
==================== LCD on MCP23008 defines ====================================

WIRING DESCRIPTION, SIGNAL NAMES
=============================================
MCP23008  LCD
---------------------------------------------
  GP0:    LITE
  GP1:    RS  - Low->command, High->data
  GP2:    R/W - Low->Write, High->Read
  GP3:    EN  - Enable
  GP4:    DB4 - Data
  GP5:    DB5 - Data
  GP6:    DB6 - Data
  GP7:    DB7 - Data
=============================================


WIRING DESCRIPTION, PIN NUMBERS
_______________________________________________________
GPIO |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0   |
PIN# | 17  | 16  | 15  | 14  | 13  | 12  | 11  | 10   |
-------------------------------------------------------
PIN# | 14  | 13  | 12  | 11  | 6   | 5   | 4   | N/A  |
LCD  | D7  | D6  | D5  | D4  | EN  | RW  | RS  | LITE |
_______________________________________________________

LCD pins D0-D3 should float or be pulled up; definitely do not ground them.

==================================================================================
*/


#include <util/delay.h>
#include "mcp23008.h"

// Map the LCD pin name to MCP23008 GPIO register bit
#define LCDLITEPIN 0b00000001	// Backlight on/off
#define LCDRSPIN   0b00000010	// Register select
#define LCDRWPIN   0b00000100	// Read-write
#define LCDENPIN   0b00001000	// Enable
#define LCDDB4PIN  0b00010000	// DB4
#define LCDDB5PIN  0b00100000	// DB5
#define LCDDB6PIN  0b01000000	// DB6
#define LCDDB7PIN  0b10000000	// DB7

#define LCDCOMMAND    1
#define LCDCHARACTER  0

// Function prototypes
void LCD_backlight(unsigned char, unsigned char);
void LCD_clear(unsigned char);
void LCD_gotoLine(unsigned char, unsigned char);
void LCD_init(unsigned char);
void LCD_pulseENPin(unsigned char);
void LCD_write4Bits(unsigned char, unsigned char, unsigned char);
void LCD_write8Bits(unsigned char, unsigned char, unsigned char);
void LCD_writeStr(unsigned char, char *);
