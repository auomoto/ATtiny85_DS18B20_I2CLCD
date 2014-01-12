#include "1wire.h"

/*============================================================================

    void oneWireInit(void)

    Set the one wire pin to be an output, HIGH. I'm not sure this is really
    needed but it's probably a good idea to make sure the one wire line
    is HIGH to start.

============================================================================*/
void oneWireInit()
{

	ONEWIREPORT |= _BV(ONEWIREPIN);
	ONEWIREDDR |= _BV(ONEWIREPIN);

}


/*============================================================================

    void oneWireMatchRom(uint8_t *addr)

    Send a match rom command to the device at addr.

============================================================================*/
void oneWireMatchRom(addr)
uint8_t *addr;
{

	uint8_t i;

	oneWireReset();
	oneWireWriteByte(0x55);
	for (i = 0; i < 8; i++) {
		oneWireWriteByte(*addr++);
	}
}

/*============================================================================

    uint8_t oneWireReaBit(void)

    Reads a bit on the one wire line. This can be usefui to know when a
    DS18B20 is finished with a convert command.

============================================================================*/
uint8_t oneWireReadBit()
{

	uint8_t state;
	
	state = 0;
	
	ONEWIREPORT |= _BV(ONEWIREPIN);
	ONEWIREDDR |= _BV(ONEWIREPIN);
	ONEWIREPORT &= ~(_BV(ONEWIREPIN));
	_delay_us(1);
	ONEWIREPORT |= _BV(ONEWIREPIN);
	ONEWIREDDR |= ~(_BV(ONEWIREPIN));
	_delay_us(8);
	if (PINB & _BV(ONEWIREPIN)) {
		state = 1;
	}
	_delay_ms(51);
	ONEWIREPORT |= _BV(ONEWIREPIN);
	ONEWIREDDR |= _BV(ONEWIREPIN);
	_delay_ms(1);
	return(state);

}

/*============================================================================

    uint8_t oneWireReadByte(void)

    Reads a byte on the one wire line. This assumes that a reset and ROM
    command have already been sent and we're waiting for one or more bytes
    to be sent by the slave device

============================================================================*/
uint8_t oneWireReadByte()
{

	uint8_t i, data;

	data =0x00;
	for (i = 0; i < 8; i++) {
		ONEWIREPORT |= _BV(ONEWIREPIN);
		ONEWIREDDR |= _BV(ONEWIREPIN);
		ONEWIREPORT &= ~(_BV(ONEWIREPIN));
		_delay_us(1);
		ONEWIREPORT |= _BV(ONEWIREPIN);
		ONEWIREDDR &= ~(_BV(ONEWIREPIN));
		_delay_us(8);
		data >>= 1;
		if (PINB & _BV(ONEWIREPIN)) {
			data |= 0b10000000;
		}
		_delay_us(51);
	}
	ONEWIREPORT |= _BV(ONEWIREPIN);
	ONEWIREDDR |= _BV(ONEWIREPIN);
	_delay_us(1);
	return(data);

}

/*============================================================================

    uint8_t oneWireReset(void)

    Holds the one wire line low for 480us, then listens for a presence
    pulse (a one wire slave device pulling the line low).

============================================================================*/
uint8_t oneWireReset()
{

	uint8_t status;
	
	status = 0;
	ONEWIREPORT &= ~(_BV(ONEWIREPIN));		// OneWire pin LOW (OR SHOULD WE MAKE A RULE THAT ITS ALWAYS AN OUTPUT?
	ONEWIREDDR |= _BV(ONEWIREPIN);			// OneWire pin becomes output
	_delay_us(480);							// Wait 480us
	ONEWIREPORT |= _BV(ONEWIREPIN);			// OneWire pin HIGH
	ONEWIREDDR &= ~(_BV(ONEWIREPIN));		// Set OneWire pin input
	_delay_us(70);							// Wait
	status = (!(PINB & _BV(ONEWIREPIN)));	// Read the presence pulse
	_delay_us(410);							// Finish reset cycle
	ONEWIREPORT |= _BV(ONEWIREPIN);			// Set OneWire pin HIGH
	ONEWIREDDR |= _BV(ONEWIREPIN);			// Set OneWire pin to output
	return(status);

}

/*============================================================================

    uint8_t oneWireReadRom(uint8_t *addr)

    If there's a single one wire device on the line, this reads its address.
    If there is more than on device, this doesn't work. addr must be an
    array of at least 8 bytes.

============================================================================*/
void oneWireReadRom(addr)
uint8_t *addr;
{

	uint8_t i;

	oneWireReset();
	oneWireWriteByte(0x33);
	for (i = 0; i < 8; i++) {
		*addr++ = oneWireReadByte();
	}

}

/*============================================================================

    uint8_t oneWireSkipRom(uint8_t *addr)

    Use this before sending a command to all devices.

============================================================================*/
void oneWireSkipRom()
{

	oneWireReset();
	oneWireWriteByte(0xCC);

}

/*============================================================================

    void oneWireWriteByte(uint8_t *addr)

    Write a byte on the one wire line.

============================================================================*/
void oneWireWriteByte(data)
uint8_t data;
{

	uint8_t i;

	for (i = 0; i < 8; i++) {
		ONEWIREPORT &= ~(_BV(ONEWIREPIN));
		_delay_us(1);
		if (data & 0x01) {
			ONEWIREPORT |= _BV(ONEWIREPIN);
			_delay_us(58);
		} else {
			_delay_us(58);
			ONEWIREPORT |= _BV(ONEWIREPIN);
		}
		_delay_us(2);
		data >>= 1;
	}
	ONEWIREPORT |= _BV(ONEWIREPIN);
	ONEWIREPORT |= _BV(ONEWIREPIN);

}