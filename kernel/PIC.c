#include "PIC.h"

void PIC_sendEOI(__int8 irq)
{
	if (irq >= 8)
		__outbyte(PIC2_COMMAND, PIC_EOI);

	__outbyte(PIC1_COMMAND, PIC_EOI);
}


/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	__int8 a1, a2;

	a1 = __inbyte(PIC1_DATA);                        // save masks
	a2 = __inbyte(PIC2_DATA);

	__outbyte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	__outbyte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	__outbyte(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	__outbyte(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	__outbyte(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	__outbyte(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	__outbyte(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	__outbyte(PIC2_DATA, ICW4_8086);
	io_wait();

	__outbyte(PIC1_DATA, a1);   // restore saved masks.
	__outbyte(PIC2_DATA, a2);
}

void pic_disable(void) {
	__outbyte(PIC1_DATA, 0xff);
	__outbyte(PIC2_DATA, 0xff);
}

void IRQ_set_mask(__int8 IRQline) {
	__int16 port;
	__int8 value;

	if (IRQline < 8) {
		port = PIC1_DATA;
	}
	else {
		port = PIC2_DATA;
		IRQline -= 8;
	}
	value = __inbyte(port) | (1 << IRQline);
	__outbyte(port, value);
}

void IRQ_clear_mask(__int8 IRQline) {
	__int16 port;
	__int8 value;

	if (IRQline < 8) {
		port = PIC1_DATA;
	}
	else {
		port = PIC2_DATA;
		IRQline -= 8;
	}
	value = __inbyte(port) & ~(1 << IRQline); // this 011111 .. means enables int line on port
	__outbyte(port, value);
}

/* Helper func */
static __int16 __pic_get_irq_reg(int ocw3)
{
	/* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
	 * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
	__outbyte(PIC1_COMMAND, ocw3);
	__outbyte(PIC2_COMMAND, ocw3);
	return (__inbyte(PIC2_COMMAND) << 8) | __inbyte(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
__int16 pic_get_irr(void)
{
	return __pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
__int16 pic_get_isr(void)
{
	return __pic_get_irq_reg(PIC_READ_ISR);
}

void io_wait() {
	__inbyte(0x80);
}
