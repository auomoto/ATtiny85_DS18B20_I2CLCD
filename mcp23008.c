/*==================== MCP23008 routines ====================================*/

#include <string.h>			// for memset()
#include "usi_twi_master.h"
#include "mcp23008.h"

/*-----------------------------------------------------------------------------

	uint8_t MCP23008Init(uint8_t addr)
	
	Initializes an MCP23008. addr is the 3-bit address of the MCP23008 being
	initialized. This routine writes all eleven registers with zeros.

-----------------------------------------------------------------------------*/
uint8_t mcp23008_Init(uint8_t addr)
{

	uint8_t buf[12], retcode;

//	usi_twi_master_initialize() must have been called before entering this routine.

	memset(buf, 0, 12);
	buf[0] = ((MCP23008_BASEADDR | addr) << 1);		// Shift mcp23008 address, set write enable bit
	usi_twi_master_start();
	retcode = usi_twi_master_transmit(buf, 12);
	usi_twi_master_stop();
	return(retcode);

}	


/*-----------------------------------------------------------------------------

	uint8_t mcp23008Read(uint8_t addr, uint8_t register)
	
	Reads an MCP23008 register. addr is the 3-bit address of the MCP23008,
	right justified, register is the MCP23008 register to read. The register
	contents are returned.

-----------------------------------------------------------------------------*/
uint8_t mcp23008_Read(addr, reg)
uint8_t addr, reg;
{

	uint8_t buf[2];

	buf[0] = ((MCP23008_BASEADDR | addr) << 1);	// Make room for r/w bit
	buf[0] |= 0x01;									// Set low bit for read operation
	buf[1] = reg;									// Read this MCP23008 register
	usi_twi_master_start();
	usi_twi_master_receive(buf, 2);
	usi_twi_master_stop();
	return(buf[1]);

}

/*-----------------------------------------------------------------------------

	uint8_t mcp23008Write(uint8_t addr, uint8_t register, uint8_t data)
	
	Writes a single register on the MCP23008. addr is the 3-bit address of the
	MCP23008, register is the register to write, and data is the byte to
	be written. Returns 0 on failure.

-----------------------------------------------------------------------------*/
uint8_t mcp23008_Write(addr, reg, dat)
uint8_t addr, reg, dat;
{

	uint8_t buf[3], retcode;

	buf[0] = ((MCP23008_BASEADDR | addr) << 1);
	buf[1] = reg;
	buf[2] = dat;
	usi_twi_master_start();
	retcode = usi_twi_master_transmit(buf, 3);
	usi_twi_master_stop();
	return(retcode);

}
