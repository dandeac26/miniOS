#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "main.h"

/// DEFINES

#define IDT_MAX_DESCRIPTORS 256 // my value
#define IDT_SIZE 256 // osdev value
#define TRUE 1

#define GDT_OFFSET_KERNEL_CODE 0x08


/// TYPES

#pragma pack(push, 1)
typedef struct _IDT_ENTRY
{
    // 15:0
    WORD            LowWordOffset;              // Bits 15:0 of address

    // 31:16
    WORD            SegmentSelector;

    // 34:32
    // IST = Interrupt Stack Table
    // if set to 0 will use legacy stack switching
    // else will use IST entry from TSS
    WORD            IST : 3;
    WORD            Reserved0 : 5;    // these must be 0 on x64
    WORD            Type : 4;    // you want interrupt gates, see Table 3-2 System-Segment and Gate-Descriptor Types
    WORD            Reserved1 : 1;    // 0
    WORD            DPL : 2;
    WORD            Present : 1;
    WORD            HighWordOffset;     // Bits 31:16 of address
    DWORD           HighestDwordOffset; // Bits 63:32 of address
    DWORD           Reserved;
} idt_entry_t;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef struct {
	__int16	limit;
	__int64	base;
} idtr_t;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef struct {
    __int64 error_code;
    __int64 rip;
    __int64 cs;
    __int64 rflags;
    __int64 rsp;
    __int64 ss;
} INTERRUPT_STACK_COMPLETE;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef struct {
    __int64 gs;
    __int64 fs;
    __int64 es;              // Extra Segment Selector (ES)
    __int64 ds;              // Data Segment Selector (DS)
    __int64 ss;              // Stack Segment Selector (SS)
    __int64 cs;              // Code Segment Selector (CS)
    __int64 rflags;          // RFLAGS register
    __int64 r15;             // General-purpose register R15
    __int64 r14;             // General-purpose register R14
    __int64 r13;             // General-purpose register R13
    __int64 r12;             // General-purpose register R12
    __int64 r11;             // General-purpose register R11
    __int64 r10;             // General-purpose register R10
    __int64 r9;              // General-purpose register R9
    __int64 r8;              // General-purpose register 
    __int64 rbp;             // Base Pointer (RBP)
    __int64 rdi;             // General-purpose register RDI
    __int64 rsi;             // General-purpose register RSI
    __int64 rdx;             // General-purpose register RDX
    __int64 rcx;             // General-purpose register RCX
    __int64 rbx;             // General-purpose register RBX
    __int64 rax;             // General-purpose register RAX
} COMPLETE_PROCESSOR_STATE;
#pragma pack(pop)



extern void* isr_stub_table[];
extern void* irq_stub_table[];

__declspec(align(16))
	static idt_entry_t idt[IDT_SIZE];

__declspec(align(16))
    extern idtr_t idtr;

static int vectors[IDT_MAX_DESCRIPTORS];

void isr_pit_c();

///  FUNCTIONS

void InterruptCommonHandler(
	__int8 InterruptIndex,   // [0x0, 0xFF
	__int8 ErrorCodeAvailable, // 0 if not available
	COMPLETE_PROCESSOR_STATE* ProcessorState, // Pointer to a structure which contains trap context (see above trap frame dump example)
    INTERRUPT_STACK_COMPLETE* StackPointer //  // Pointer to Stack Pointer After Transfer to Handler (Fig 6-9)
);


void idt_set_descriptor(__int8 vector, void* isr, __int8 flags);
void idt_init(void);

void keyboard_interrupt_handler();

#endif _INTERRUPTS_H_

