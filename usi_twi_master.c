#include "usi_twi_master.h"

/* SDA is always returned to OUTPUT state */


/*
	Initialize USI for TWI. This guarantees the SCL and SDA pins
	are set up as outputs and will have pullups when used as inputs.
	2013-11-23/au.
*/
void usi_twi_master_initialize(void)
{

	DDR_USI |= _BV(PIN_USI_SCL);		// Set SCL pin as output
	DDR_USI |= _BV(PIN_USI_SDA);		// Set SDA pin as output
	PORT_USI |= _BV(PIN_USI_SCL);		// Enable pullup on SCL
	PORT_USI |= _BV(PIN_USI_SDA);		// Enable pullup on SDA

}

/*
	A start condition must precede this call. The first byte must
	be a properly formatted I2C address with R/W (bit 0) set to 1.
	Returns the data in the supplied array; the first byte of the
	array is not changed. 2013-11-23/au.
*/
void usi_twi_master_receive(uint8_t *data, uint8_t nbytes)
{

	usi_twi_master_transmit(data++, 1);			// Send the address
	nbytes--;
	do {
		DDR_USI &= ~_BV(PIN_USI_SDA);				// Set SDA as input
		*(data++)  = usi_twi_master_transfer(8);	// Read a byte
		DDR_USI |= _BV(PIN_USI_SDA);				// Set SDA as output
		USIDR = (nbytes == 1 ? 0xFF : 0x00);		// Load NACK if last byte
		usi_twi_master_transfer(1);					// Send ACK or NACK
	} while (--nbytes);

}

/*
	Puts a start condition on the bus. Taken from the AVR310 routines
	(pulled out of USI_TWI_Start_Transceiver_With_Data).
*/
void usi_twi_master_start(void)
{

	PORT_USI |= _BV(PIN_USI_SCL);					// Release SCL
	while( !(PORT_USI & _BV(PIN_USI_SCL)) ) {		// Wait for clock stretching to end
	}
	_delay_us(T_LOW);								// Setup time for START
	PORT_USI &= ~_BV(PIN_USI_SDA);					// Pull SDA LOW
	_delay_us(T_HIGH);								// Wait
	PORT_USI &= ~_BV(PIN_USI_SCL);					// Pull SCL LOW
	PORT_USI |= _BV(PIN_USI_SDA);					// Release SDA (START signal)

}


/*
	Puts a stop condition on the bus. Taken from the AVR310 routines
	and cleaned up a bit. 2013-11-21/au.
*/
void usi_twi_master_stop(void)
{

	PORT_USI &= ~_BV(PIN_USI_SDA);				// Pull SDA low
	PORT_USI |= _BV(PIN_USI_SCL);				// Release SCL
	while(!(PIN_USI & _BV(PIN_USI_SCL))) {		// Wait for clock stretching to end
	}
	_delay_us(T_HIGH);							// Setup time for STOP
	PORT_USI |= _BV(PIN_USI_SDA);				// Release SDA (STOP signal)
	_delay_us(T_LOW);                			// Time to hold STOP

}

/*
	Called by usi_twi_master_transmit and usi_twi_master_receive.
	Load USIDR with data for write operations. 2013-11-23/au.
*/

uint8_t usi_twi_master_transfer(uint8_t nbits)
{

	uint8_t data;

	// Select a 1-bit or 8-bit data transfer
	USISR = (nbits == 1 ? USISR_1_BIT : USISR_8_BIT);
	do {
		_delay_us(T_LOW);				// Set up for transfer
		USICR = USICR_SETUP;			// Generate rising SCL edge
		while(!(PIN_USI & _BV(PIN_USI_SCL))) {	//Wait for SCL to go high
		}
		_delay_us(T_HIGH);				// USI will read the bit
		USICR = USICR_SETUP;			// Generates falling SCL edge
	} while(!(USISR & _BV(USIOIF)));	// Until 4-bit counter overflow. Both rising
										// and falling edges increment the counter
	_delay_us(T_LOW);
	data = USIDR;					//Read out data
	DDR_USI |= _BV(PIN_USI_SDA);	//Enable SDA as output
	USIDR = 0xFF;					//Release SDA. Bit 7 is the SDA pin output latch. Doesn't work w/o this line.
	return(data);					//Return received data

}

/*
	A start condition must precede this call. The first byte must
	be a properly formatted I2C address with R/W (bit 0) set to 0.
	Returns 1 (TRUE) on success, 0 (FALSE) otherwise. If FALSE, you
	should abort the transfer (send a stop) before proceeding.
	Extracted from USI_TWI_Start_Transceiver_With_Data (AVR310).
	2013-11-23/au.
*/
uint8_t usi_twi_master_transmit(uint8_t *data, uint8_t nbytes)
{

	do {	
		PORT_USI &= ~_BV(PIN_USI_SCL);					// Set SCL low
		USIDR = *(data++);								// Load data byte
		usi_twi_master_transfer(8);						// Send 8 bits on bus
		DDR_USI &= ~_BV(PIN_USI_SDA);					// Enable SDA as input
		if ((usi_twi_master_transfer(1) & (0x01))) {	// If slave doesn't ACK
			DDR_USI |= _BV(PIN_USI_SDA);				// Set SDA as output
			return(FALSE);
		}
	} while (--nbytes);
	DDR_USI |= _BV(PIN_USI_SDA);						// Set SDA as output
	return(TRUE);

}
