/*	ATtiny85Learn2

	Chip details:
	
	FUSES: Low:0x62, High:0xDF, Extended:0xFF
		Low=0x62 is 8MHz clock with /8 divider (1MHz CPU clock)
		High=0xDF enables programming, not much else
		Extended=0xFF (no self programming)
 
	To read fuses (lfuse, hfuse, or efuse):
		avrdude -c usbtiny -p attiny85 -U lfuse:r:-:h

	To write fuses (example):
		avrdude -c usbtiny -p attiny85 -U lfuse:w:0x62:m
		
=========================================================
		
	STEP 1: Set up timer interrupts and light the LED.
		The init_ATtiny85() routine sets up the LED on pin 2,
		sets the timer0 clock to run at 1/1024 of the CPU clock
		(so it's 1KHz), and sets up timer interrupts using
		clear-timer-on-compare (CTC) using timer0. The value
		to compare is loaded into the output compare register
		OCR0A (8-bits).

	TTCR0A = Timer/Counter Control Register A for timer0 (set clear timer on compare, CTC, mode)
	TTCR0B = Timer/Counter Control Register B for timer0 (set prescaler by setting CS02, CS01, CS00)
		----------------------
		| CS02 | CS01 | CS00 |
		----------------------
		|   1  |   0  |   1  | is /1024
		----------------------

=========================================================

	STEP 2: Set up TWI communications.
	
		The MCP23008 base address is 0x20. Insert the device address (3 bits),
		shift this left one bit and insert the r/w bit to form the address.
		The base address is handled within MCP23008Write(addr, REGISTER, data)
		so you only put in the 3-bit address as addr.

		Testing the TWI communications is done with the test lights board, an
		LED light bar with eight (out of ten) LEDs connected to the MCP23008.


=========================================================

	STEP 3: Connect the LCD display. The LCD is driven off an MCP23008 at
		address 0x01.

=========================================================

	STEP 4: Set up the OneWire communications.

=========================================================

	STEP 5: Set up the DS18B20 routines.

=========================================================


*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "usi_twi_master.h"
#include "mcp23008.h"
#include "mcp23008LCD.h"
#include "1wire.h"
#include "ds18b20.h"

#define LEDPIN	PB3					// PB3 is pin 2 on the ATtiny85
#define MCP23008_ADDR_LCD			0x01	// MCP23008 I2C address (add to base address)

// Function prototypes
void init_ATtiny85(void);
void LED_Flash(uint8_t nflash);

// Globals
volatile uint8_t CounterLoops;
volatile uint8_t ds18b20addr[8];
volatile int16_t lastTemp;

int main(void)
{

	init_ATtiny85();
	OCR0A = 255;						// Output Compare Register A for timer0, 8-bit
	sei();
	for (;;) {
	}

}

void init_ATtiny85()
{

	uint8_t i;
	char buffer[5];
	CounterLoops = 0;

	DDRB |= _BV(LEDPIN);				// Set the LED pin to output mode

	// Setup timer interrupts
	TCCR0B |= (_BV(CS02) | _BV(CS00));	// Prescale timer0 to /1024 of the cpu clock (976.6 Hz)
	TCCR0A |= _BV(WGM01);				// Set "clear timer on compare match (CTC)" mode
	TIMSK |= _BV(OCIE0A);				// Enable interrupt for Timer/Counter Output Compare Match A


	usi_twi_master_initialize();		// Initialize the TWI interface
	mcp23008_Init(MCP23008_ADDR_LCD);	// Initialize the mcp23008 port expander

	LCD_init(MCP23008_ADDR_LCD);		// Initialize the LCD
	LCD_backlight(MCP23008_ADDR_LCD, 1);
	LCD_writeStr(MCP23008_ADDR_LCD, "20131123");

	oneWireInit();								// Initialize the OneWire net
	oneWireReadRom((uint8_t*) ds18b20addr);	// Get the address of the DS18B20

	// Write the address of the only DS18B20 device on the onewire net
	LCD_clear(MCP23008_ADDR_LCD);
	LCD_gotoLine(MCP23008_ADDR_LCD, 1);
	for (i = 0; i < 4; i++) {
		sprintf(buffer, "%02X", ds18b20addr[i]);
		LCD_writeStr(MCP23008_ADDR_LCD, buffer);
	}
	LCD_gotoLine(MCP23008_ADDR_LCD, 2);
	for (i = 4; i < 8; i++) {
		sprintf(buffer, "%02X", ds18b20addr[i]);
		LCD_writeStr(MCP23008_ADDR_LCD, buffer);
	}
	_delay_ms(2000);

}

void LED_Flash(uint8_t nflash)
{

	uint8_t i;
	
	for (i = 0; i < nflash; i++) {
		PORTB |= _BV(LEDPIN);
		_delay_ms(1000);
		PORTB &= ~(_BV(LEDPIN));
		_delay_ms(1000);
	}
}

ISR(TIMER0_COMPA_vect)
{

	char str[8];
	int itemperature;
	double Celsius, Fahrenheit;

	if (++CounterLoops == 10) {

		CounterLoops = 0;

		// Flash the ATtiny85 LED
		PORTB ^= _BV(LEDPIN);

		// Get the temperature from the DS18B20
		itemperature = ds18b20GetTemperature((uint8_t*) ds18b20addr);

		// Don't do anything if the temperature is the same
		if (itemperature == lastTemp) {
			return;
		}

		// Compute the new temperature
		lastTemp = itemperature;
		Celsius = 0.0625 * (double) itemperature;
		Fahrenheit = 32.0 + ((9.0 * Celsius) / 5.0);

		// Display the new temperature on the LCD
		LCD_clear(MCP23008_ADDR_LCD);
		sprintf(str, "%4.1f C", Celsius);
		LCD_gotoLine(MCP23008_ADDR_LCD, 1);
		LCD_writeStr(MCP23008_ADDR_LCD, str);
		sprintf(str, "%4.1f F", Fahrenheit);
		LCD_gotoLine(MCP23008_ADDR_LCD, 2);
		LCD_writeStr(MCP23008_ADDR_LCD, str);

	}
}
