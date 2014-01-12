ATtiny85_TWI_MCP23008_DS18B20_LCD
=================================

This reads a DS18B20 1-Wire temperature sensor and prints the temperature on an LCD screen controlled by an I2C MCP23008 port expander.

Work in progress; this might not work. (see the TWI_SCAN project for later versions?)

ATtiny85 details:

FUSES:	Low:0x62, High:0xDF, Extended:0xFF
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

STEP 2: Set up I2C communications. For I2C, there is one #define
	(__ATtiny85__) and one #include file (usi_twi_master.c). Initialize
	the I2C interface with a call to usi_twi_master_initialize().

	The MCP23008 base address is 0x20. Insert the device address (3 bits),
	shift this left one bit and insert the r/w bit to form the address.
	The base address is handled within MCP23008Write(addr, REGISTER, data)
	so you only put in the 3-bit address as addr.
		Testing the I2C communications is done with the test lights board, an
	LED light bar with eight (out of ten) LEDs connected to the MCP23008.

STEP 2: Rev C.

=========================================================

STEP 3: Connect the LCD display. The LCD is driven off an MCP23008 at
	address 0x01.

=========================================================

STEP 4: Set up the OneWire communications.

=========================================================

STEP 5: Set up the DS18B20 routines.

=========================================================

Note: The usi_twi_master routines are old. Newer versions are in the ATtiny4313 PCA9543 project directory and should replace these. Also change the MCP23008 LCD routines to match the Adafruit backpack pinning since we'll probably buy more of those instead of making our own.

