#include "interrupts.h"
#include "scancode.h"
#include "console.h"
#include "screen.h"

idtr_t idtr;
void* isr_stub_table[];
void* irq_stub_table[];
extern void* pit_isr_stub;

static int tick_count = 0;


/// ADJUSTED TO LAB CODE
void idt_set_descriptor(__int8 vector, void* isr, __int8 flags) {
    idt_entry_t* descriptor = &idt[vector];

    // Set the low part of the ISR address (bits 0–15)
    descriptor->LowWordOffset = (__int64)isr & 0xFFFF;

    // Set the GDT segment selector
    descriptor->SegmentSelector = GDT_OFFSET_KERNEL_CODE;

    // Set the IST field to 0 (use legacy stack switching)
    descriptor->IST = 0;

    // Set the gate type and flags
    descriptor->Type = 0xE; // Interrupt gate type
    descriptor->DPL = (flags >> 5) & 0x3;   // Extract DPL bits from flags /// always 00 when flags = 0x8E
    descriptor->Present = 1;                // Entry should be present

    // Set the middle part of the ISR address (bits 16–31)
    descriptor->HighWordOffset = ((__int64)isr >> 16) & 0xFFFF;

    // Set the high part of the ISR address (bits 32–63)
    descriptor->HighestDwordOffset = ((__int64)isr >> 32) & 0xFFFFFFFF;

    // Set the remaining reserved field to 0
    descriptor->Reserved = 0;
}



void isr_pit_c()
{
    tick_count++;

    // Check if 2 seconds (100 ticks at 100 Hz) have passed
    //if (tick_count % 200 == 0) {
    //    //LogSerialAndScreen("test with tick = %d\n", tick_count);
    //}
    __send_EOI();
}

int GetTimeTillBootSeconds() {
    if (tick_count == 0) return 1;
    return tick_count / 100; // 100 ticks = 1 second
}

void GetTimeTillBoot(int* minutes, int* seconds) {
    int total_seconds = GetTimeTillBootSeconds();
     *minutes = total_seconds / 60;
     *seconds = total_seconds % 60;
}


#pragma optimize("", off)
void idt_init() {
    
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (__int16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (__int8 vector = 0; vector < 32; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
		vectors[vector] = TRUE;
	}

    idt_set_descriptor(32, __pit_isr_stub_handler, 0x8E);
    idt_set_descriptor(33, __kb_isr_stub_handler, 0x8E);

    ///REST DONE IN __init.asm
}
#pragma optimize("", on)


void InterruptCommonHandler(
    __int8 InterruptIndex,
    __int8 ErrorCodeAvailable,
    COMPLETE_PROCESSOR_STATE* ProcessorState,
    INTERRUPT_STACK_COMPLETE* StackPointer
) {
   
    LogSerialAndScreen("\Interrupt Index: %d\nHas Error: %d\n-----------\nCOMPLETE_PROCESSOR_STATE: \nrax:0x%X | rbx:0x%X | rcx:0x%X | rdx:0x%X\nrsi:0x%X | rdi:0x%X | rbp:0x%X | r8:0x%X\nr9:0x%X | r10:0x%X | r11:0x%X | r12:0x%X\nr13:0x%X | r14:0x%X | r15:0x%X\nrflags:0x%X\ncs:0x%X | ss:0x%X | ds:0x%X | es:0x%X\n-----------\nSTACK DATA: \nError Code:0x%D | RIP:0x%X | CS:0x%X | RFLAGS:0x%X | RSP:0x%X | SS:0x%X\n",
        InterruptIndex,
        ErrorCodeAvailable,
        ProcessorState->rax,
        ProcessorState->rbx,
        ProcessorState->rcx,
        ProcessorState->rdx,
        ProcessorState->rsi,
        ProcessorState->rdi,
        ProcessorState->rbp,
        ProcessorState->r8,
        ProcessorState->r9,
        ProcessorState->r10,
        ProcessorState->r11,
        ProcessorState->r12,
        ProcessorState->r13,
        ProcessorState->r14,
        ProcessorState->r15,
        ProcessorState->rflags,
        ProcessorState->cs,
        ProcessorState->ss,
        ProcessorState->ds,
        ProcessorState->es,
        
        StackPointer->error_code,
        StackPointer->rip,
        StackPointer->cs,
        StackPointer->rflags,
        StackPointer->rsp,
        StackPointer->ss
    );

    __magic();
    //__haltt(); // MIGHT BE NEEDED

    
    if (ErrorCodeAvailable) {

    }
}


static int is_extended = 0;


void keyboard_interrupt_handler_c() {
    unsigned char scancode;
    scancode = __inbyte(0x60);
    
    if (scancode == 0xE0 || scancode == 0xE1) 
    {
        is_extended = 1;
        __send_EOI();
        return;
    }

    
    if (scancode == 0xAA || scancode == 0xB6)
    {
        shiftKeyDown = false; // Shift key 
    }

    KEYCODE key;

    if (is_extended) 
    {
        key = _kkybrd_scancode_ext[scancode];
        if (key != KEY_UNKNOWN) 
        {
            PutCharExt(key);  
        }
        is_extended = 0;  
    }
    else 
    {
        // Handle as a standard scancode
        key = _kkybrd_scancode_std[scancode];
        if (key != KEY_UNKNOWN || key == (KEYCODE)(0xAA) || key == (KEYCODE)(0xB6)) 
        {
            PutCharStd(key);  
        }
    }

    __send_EOI();  
}