#include "ds18b20.h"

void ds18b20ConvertT()
{

	oneWireWriteByte(0x44);
	while (! oneWireReadBit()) {
	}

}

int ds18b20GetTemperature(addr)
uint8_t *addr;
{

	uint8_t scratchpad[2];
	uint16_t temperature;

	oneWireReset();
	oneWireMatchRom(addr);
	ds18b20ConvertT();

	oneWireReset();
	oneWireMatchRom(addr);
	ds18b20ReadScratchpad(scratchpad, 2);
	temperature = scratchpad[1];
	temperature <<= 8;
	temperature |= scratchpad[0];
	return(temperature);

}

void ds18b20ReadScratchpad(scratchpad, nbytes)
uint8_t *scratchpad, nbytes;
{

	uint8_t i;

	oneWireWriteByte(0xBE);
	for (i = 0; i < nbytes; i++) {
		*scratchpad++ = oneWireReadByte();
	}
}