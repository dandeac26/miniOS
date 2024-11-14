
#ifndef _PIC_H_
#define _PIC_H_

#include "main.h"
#include "interrupts.h"

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */


#define PIT_FREQ 1193180
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42

#define PIT_CMD 0x43

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_READ_IRR 0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b    /* OCW3 irq service next CMD read */

void PIC_sendEOI(__int8 irq);
void PIC_remap(int offset1, int offset2);
void pic_disable(void);

void IRQ_set_mask(__int8 IRQline);
void IRQ_clear_mask(__int8 IRQline);

__int16 pic_get_irr(void);
__int16 pic_get_isr(void);


void io_wait();

#endif _PIC_H_



