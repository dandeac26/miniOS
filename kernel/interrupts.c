#include "interrupts.h"
#include "main.h"

//void exception_handler() {
//	__asm volatile ("cli; hlt"); // Completely hangs the computer
//}

idtr_t idtr;
void* isr_stub_table[];

/// OSDEV TUTORIAL
//void idt_set_descriptor(__int8 vector, void* isr, __int8 flags) {
//	idt_entry_t* descriptor = &idt[vector];
//
//	descriptor->isr_low = (__int64)isr & 0xFFFF;
//	descriptor->kernel_cs = GDT_OFFSET_KERNEL_CODE;
//	descriptor->ist = 0;
//	descriptor->attributes = flags;
//	descriptor->isr_mid = ((__int64)isr >> 16) & 0xFFFF;
//	descriptor->isr_high = ((__int64)isr >> 32) & 0xFFFFFFFF;
//	descriptor->reserved = 0;
//}

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



#pragma optimize("", off)
void idt_init() {
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (__int16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (__int8 vector = 0; vector < 32; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
		vectors[vector] = TRUE;
	}

    //__loadIDT();
    /// DONE IN __init.asm
	//__asm volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
	//__asm volatile ("sti"); // set the interrupt flag
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
    //__print_msg();
    __magic();
    __haltt();



    //switch (InterruptIndex) {
    //case 0: // Divide by Zero Error
    //    // Handle the divide by zero error
    //    

    //    LogSerialAndScreen("\ndiv0");
    //    LogSerialAndScreen("rax:0x%X\nrbx:0x%X\nrcx:0x%X\nrdx:0x%X\nInterrupt index: %d\nHas error code: %d",
    //        ProcessorState->rax,
    //        ProcessorState->rbx,
    //        ProcessorState->rcx,
    //        ProcessorState->rdx,
    //        InterruptIndex,
    //        ErrorCodeAvailable
    //    );
    //    //__print_msg();
    //    __magic();
    //    __haltt();
    //    break;
    //case 1: // Debug Exception
    //    

    //    LogSerialAndScreen("\nException1");
    //    LogSerialAndScreen("rax:0x%X\nrbx:0x%X\nrcx:0x%X\nrdx:0x%X\nInterrupt index: %d\nHas error code: %d",
    //        ProcessorState->rax,
    //        ProcessorState->rbx,
    //        ProcessorState->rcx,
    //        ProcessorState->rdx,
    //        InterruptIndex,
    //        ErrorCodeAvailable
    //    );
    //    //__print_msg();
    //    __magic();
    //    __haltt();



    //    break;


    //case 2:
    //    LogSerialAndScreen("\nException3");
    //    LogSerialAndScreen("rax:0x%X\nrbx:0x%X\nrcx:0x%X\nrdx:0x%X\nInterrupt index: %d\nHas error code: %d",
    //        ProcessorState->rax,
    //        ProcessorState->rbx,
    //        ProcessorState->rcx,
    //        ProcessorState->rdx,
    //        InterruptIndex,
    //        ErrorCodeAvailable
    //    );
    //    //__print_msg();
    //    __magic();
    //    __haltt();
    //    break;
    //case 3:
    //    LogSerialAndScreen("\nException3");
    //    LogSerialAndScreen("rax:0x%X\nrbx:0x%X\nrcx:0x%X\nrdx:0x%X\nInterrupt index: %d\nHas error code: %d",
    //        ProcessorState->rax,
    //        ProcessorState->rbx,
    //        ProcessorState->rcx,
    //        ProcessorState->rdx,
    //        InterruptIndex,
    //        ErrorCodeAvailable
    //    );
    //    //__print_msg();
    //    __magic();
    //    __haltt();
    //    break;
    //   
    //default:
    //    // Default case to handle unhandled interrupts
    //    break;
    //}

    
    if (ErrorCodeAvailable) {
        // Process the error code if needed
    }
    return;
}