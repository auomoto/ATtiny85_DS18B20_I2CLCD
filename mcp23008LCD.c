/*================== LCD on MCP23008 routines ==================================*/

#include "mcp23008LCD.h"

/*-----------------------------------------------------------------------------

	void LCD_backlight(uint8_t LCD_addr, uint8_t mode)

	Turns the backlight on or off. If mode is 0, the light is turned off.
	If mode is not 0, the light is turned on. This controls the voltage on
	the 2N7000 gate that sinks current for the LED backlight.

-----------------------------------------------------------------------------*/
void LCD_backlight(LCD_addr, mode)
uint8_t LCD_addr, mode;
{

	uint8_t gpioReg;

	gpioReg = mcp23008_Read(LCD_addr, GPIO);			// Read current GPIO register
	if (mode) {
		gpioReg |= LCDLITEPIN;
	} else {
		gpioReg &= ~LCDLITEPIN;
	}
	mcp23008_Write(LCD_addr, GPIO, gpioReg);			// Write back GPIO

}

/*-----------------------------------------------------------------------------

	void LCD_clear(uint8_t LCD_addr)

	Clears the lcd screen. LCD_addr is the 3-bit address of the MCP23008 that
	the LCD is wired to.

-----------------------------------------------------------------------------*/
void LCD_clear(LCD_addr)
uint8_t LCD_addr;
{

	LCD_write8Bits(LCD_addr, 0x01, LCDCOMMAND);
	_delay_ms(2);							// Clear function takes a long time

}


/*-----------------------------------------------------------------------------

	LCD_gotoLine(uint8_t LCD_addr, uint8_t line)
	
	Moves to the selected line number. LCD_addr is the 3-bit address of the
	MCP23008 port expander that the LCD is wired to. This is for a two-line
	display.

-----------------------------------------------------------------------------*/
void LCD_gotoLine(LCD_addr, line)
uint8_t LCD_addr, line;
{

	switch (line) {
		case 1:
			LCD_write8Bits(LCD_addr, 0x80, LCDCOMMAND);
			break;
		case 2:
			LCD_write8Bits(LCD_addr, 0xC0, LCDCOMMAND);
			break;
	}

}

/*-----------------------------------------------------------------------------

	LCD_init

	See page 46 of the HD44780U data sheet
-----------------------------------------------------------------------------*/
void LCD_init(LCD_addr)
uint8_t LCD_addr;
{

	mcp23008_Init(LCD_addr);
	_delay_ms(20);    								// Wait >40ms after 2.7V, 15ms after 4.5V for LCD
	LCD_write4Bits(LCD_addr, 0x03, LCDCOMMAND);
	_delay_us(4500);								// Wait > 4.1ms
	LCD_write4Bits(LCD_addr, 0x03, LCDCOMMAND);
	_delay_us(110);									// Wait > 100us
	LCD_write4Bits(LCD_addr, 0x03, LCDCOMMAND);
	LCD_write4Bits(LCD_addr, 0x02, LCDCOMMAND);		// Set to 4-bit mode
	LCD_write8Bits(LCD_addr, 0x28, LCDCOMMAND);		// Sets 2-line mode
	LCD_write8Bits(LCD_addr, 0x08, LCDCOMMAND);		// Display off
	LCD_clear(LCD_addr);							// Clear display
	LCD_write8Bits(LCD_addr, 0x06, LCDCOMMAND);		// Increment char, no shift
	LCD_write8Bits(LCD_addr, 0x0C, LCDCOMMAND);		// Display ON, no cursor
//	LCD_write8Bits(LCD_addr, 0x0F, LCDCOMMAND);		// Display ON, blinking cursor

}

/*-----------------------------------------------------------------------------

	void LCD_pulseENPin(uint8_t LCD_addr)
	
	Pulses to the LCD enable pin. LCD_addr is the 3-bit address of the MCP23008
	to which the LCD is wired. The LCD latches data when EN is transitioned
	from HIGH to LOW.

-----------------------------------------------------------------------------*/
void LCD_pulseENPin(LCD_addr)
uint8_t LCD_addr;
{

	uint8_t gpioReg;

	gpioReg = mcp23008_Read(LCD_addr, GPIO);		// Read current GPIO register
	gpioReg &= ~(LCDENPIN);					// Set enable bit LOW
	mcp23008_Write(LCD_addr, GPIO, gpioReg);		// Write GPIO
	_delay_us(1);								// wait >450 ns
	gpioReg |= LCDENPIN;						// Set enable bit HIGH
	mcp23008_Write(LCD_addr, GPIO, gpioReg); 	// Write GPIO
	_delay_us(1);								// Wait >450 ns
	gpioReg &= ~(LCDENPIN);					// Set enable bit LOW
	mcp23008_Write(LCD_addr, GPIO, gpioReg);		// Write GPIO
	_delay_us(37);								// Wait at least 37us

}


/*-----------------------------------------------------------------------------

	void LCD_Write4Bits(uint8_t LCD_addr, uint8_t fourBits, int mode)
	
	Write 4 bits to the LCD. LCD_addr is the 3-bit address of the MCP23008
	to which the LCD is wired. fourBits is the data; the lowermost 4 bits are
	sent to the LCD. mode is either LCDCHARACTER or LCDCOMMAND (this sets the
	register select, RS, pin).

-----------------------------------------------------------------------------*/
void LCD_write4Bits(LCD_addr, fourBits, mode)
uint8_t LCD_addr, fourBits, mode;
{
  
	uint8_t gpioReg;

	gpioReg = mcp23008_Read(LCD_addr, GPIO);
	gpioReg &= LCDLITEPIN;				// Save only the backlight status

	if (mode == LCDCHARACTER) {
		gpioReg |= LCDRSPIN;			// Register select (set for CHARACTER)
	} else {
		gpioReg &= ~LCDRSPIN;			// Register select (set for COMMAND)
	}

	// Lowest four bits of "fourBits" goes into gpioReg bits 7,6,5,4
	fourBits <<= 4;		// Shift left 4 bits to match MCP23008 pins to LCD pins
	gpioReg |= fourBits;
	mcp23008_Write(LCD_addr, GPIO, gpioReg);
	_delay_us(1);						// Must be >140 ns
	LCD_pulseENPin(LCD_addr);

}

/*-----------------------------------------------------------------------------

	void LCD_Write8Bits(uint8_t LCD_addr, uint8_t fourBits, int mode)
	
	Writes 8 bits to the LCD when the LCD is in 4-bit mode. LCD_addr is the
	3-bit address of the MCP23008 to which the LCD is wired. eightBits is the
	data. mode is either LCDCHARACTER or LCDCOMMAND (this sets the register
	select, RS, pin).

-----------------------------------------------------------------------------*/
void LCD_write8Bits(LCD_addr, eightBits, mode)
uint8_t LCD_addr, eightBits, mode;
{


	LCD_write4Bits(LCD_addr, eightBits>>4, mode);
	LCD_write4Bits(LCD_addr, eightBits, mode);

}


/*-----------------------------------------------------------------------------

	void LCD_WriteStr(uint8_t LCD_addr, char *str)
	
	Writes a string to the LCD. LCD_addr is the 3-bit address of the MCP23008
	to which the LCD is wired. str is the NUL terminated character string to
	write.

-----------------------------------------------------------------------------*/
void LCD_writeStr(LCD_addr, str)
uint8_t LCD_addr;
char str[];
{
	
	while (*str) {
		LCD_write8Bits(LCD_addr, *str++, LCDCHARACTER);
	}

}
