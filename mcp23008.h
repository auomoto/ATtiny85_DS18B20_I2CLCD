/*==================== mcp23008 defines ====================================*/

#include <avr/io.h>

#define MCP23008_BASEADDR 0x20	// Add this to the 3-bit I2C address jumpered on the chip
								// Registers:
#define IODIR   0x00			// I/O direction, 0 for output, 1 for input, 0xFF at power-on reset
#define IPOL    0x01			// Input polarity, 0 for non-inverting, 1 for inverting
#define GPINTEN 0x02			// Interrupt on change, 0 for off, 1 for on
#define DEFVAL  0x03			// Interrupt on change default comparison byte
#define INTCON  0x04			// Interrupt control: if 0, check for change, if 1; check against DEFVAL
#define IOCON   0x05			// Configuration register (see data sheet)
#define GPPU    0x06			// Pullup config; 1=enabled
#define INTF    0x07			// Interrupt flag; read-only, which pin caused the interrupt?
#define INTCAP  0x08			// Interrupt capture; GPIO state when interrupt occurred
#define GPIO    0x09			// GPIO pins
#define OLAT    0x0A			// Output latches for GPIO

// Function prototypes
uint8_t mcp23008_Init(uint8_t);
uint8_t mcp23008_Read(uint8_t, uint8_t);
uint8_t mcp23008_Write(uint8_t, uint8_t, uint8_t);
