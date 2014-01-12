#include <avr/io.h>
#include <util/delay.h>


/* 1wire defines for an ATtiny85 with the One Wire device on PB4 (pin 3) */
#define ONEWIREPORT     PORTB
#define ONEWIREPIN      PB4
#define ONEWIREDDR      DDRB

// Function prototypes
void oneWireInit(void);
void oneWireMatchRom(uint8_t *);
uint8_t oneWireReadBit(void);
uint8_t oneWireReadByte(void);
uint8_t oneWireReset(void);
void oneWireReadRom(uint8_t *);
void oneWireSkipRom(void);
void oneWireWriteByte(uint8_t);
