#include "IO.h"

unsigned read_pit_count(void) {
	unsigned count = 0;

	// Disable interrupts
	__cli();

	// al = channel in bits 6 and 7, remaining bits clear
	__outbyte(0x43, 0b0000000);

	count = __inbyte(0x40);		// Low byte
	count |= __inbyte(0x40) << 8;		// High byte

	return count;
}

void set_pit_count(unsigned count) {
	// Disable interrupts
	__cli();

	// Set low byte
	__outbyte(0x40, count & 0xFF);		// Low byte
	__outbyte(0x40, (count & 0xFF00) >> 8);	// High byte
	return;
}