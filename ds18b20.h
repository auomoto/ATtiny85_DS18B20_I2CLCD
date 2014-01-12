#include <avr/io.h>
#include "1wire.h"

void ds18b20ConvertT(void);
int ds18b20GetTemperature(uint8_t *);
void ds18b20ReadScratchpad(uint8_t *, uint8_t);
