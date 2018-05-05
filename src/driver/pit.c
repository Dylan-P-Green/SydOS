#include <main.h>
#include <io.h>
#include <kprint.h>
#include <driver/pit.h>

// Starts a counter on the PIT.
void pit_startcounter(uint32_t freq, uint8_t counter, uint8_t mode) {
	// Determine divisor.
	uint16_t divisor = PIT_BASE_FREQ / freq;

	// Send command to PIT.
	uint8_t command = 0;
	command = (command & ~PIT_CMD_MASK_BINCOUNT) | PIT_CMD_BINCOUNT_BINARY;
	command = (command & ~PIT_CMD_MASK_MODE) | mode;
	command = (command & ~PIT_CMD_MASK_ACCESS) | PIT_CMD_ACCESS_DATA;
	command = (command & ~PIT_CMD_MASK_COUNTER) | counter;
	outb(PIT_PORT_COMMAND, command);

	// Send divisor.
	switch (counter) {
		case PIT_CMD_COUNTER0:
			outb(PIT_PORT_CHANNEL0, divisor & 0xFF);
			outb(PIT_PORT_CHANNEL0, (divisor >> 8) & 0xFF);
			break;

		case PIT_CMD_COUNTER1:
			outb(PIT_PORT_CHANNEL1, divisor & 0xFF);
			outb(PIT_PORT_CHANNEL1, (divisor >> 8) & 0xFF);
			break;

		case PIT_CMD_COUNTER2:
			outb(PIT_PORT_CHANNEL2, divisor & 0xFF);
			outb(PIT_PORT_CHANNEL2, (divisor >> 8) & 0xFF);
			break;
	}
}

// Initialize the PIT.
void pit_init(void) {
	// Start main timer at 1 tick = 1 ms.
	kprintf("PIT: Initializing...\n");
	pit_startcounter(1000, PIT_CMD_COUNTER0, PIT_CMD_MODE_SQUAREWAVEGEN);
	kprintf("PIT: Initialized!\n");
}