#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "main.h"

/// DEFINES

#define IDT_MAX_DESCRIPTORS 256// my value
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

//#pragma pack(push)
//#pragma pack(1)
//typedef struct {
//	__int16    isr_low;      // The lower 16 bits of the ISR's address
//	__int16    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
//	__int8	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
//	__int8     attributes;   // Type and attributes; see the IDT page
//	__int16    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
//	__int32    isr_high;     // The higher 32 bits of the ISR's address
//	__int32    reserved;     // Set to zero
//} idt_entry_t;
//#pragma pack(pop)


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
    void* rip;               // Return Instruction Pointer (RIP)
    __int16 cs;              // Code Segment Selector (CS)
    __int64 rflags;          // RFLAGS register
    void* rsp;              // Stack Pointer (RSP)
    __int16 ss;              // Stack Segment Selector (SS)
} INTERRUPT_STACK_COMPLETE;
#pragma pack(pop)



//#pragma pack(push)
//#pragma pack(1)
//typedef struct {
//    __int32 error_code;
//    void* rip;               // Return Instruction Pointer (RIP)
//    __int16 cs;              // Code Segment Selector (CS)
//    __int64 rflags;          // RFLAGS register
//    void* rsp;              // Stack Pointer (RSP)
//    __int16 ss;              // Stack Segment Selector (SS)
//} INTERRUPT_STACK_COMPLETE;
//#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct {
    __int64 rax;             // General-purpose register RAX
    __int64 rbx;             // General-purpose register RBX
    __int64 rcx;             // General-purpose register RCX
    __int64 rdx;             // General-purpose register RDX
    __int64 rsi;             // General-purpose register RSI
    __int64 rdi;             // General-purpose register RDI
    __int64 rbp;             // Base Pointer (RBP)
    __int64 rsp;             // Stack Pointer (RSP)
    __int64 r8;              // General-purpose register R8
    __int64 r9;              // General-purpose register R9
    __int64 r10;             // General-purpose register R10
    __int64 r11;             // General-purpose register R11
    __int64 r12;             // General-purpose register R12
    __int64 r13;             // General-purpose register R13
    __int64 r14;             // General-purpose register R14
    __int64 r15;             // General-purpose register R15
    __int16 cs;              // Code Segment Selector (CS)
    __int16 ss;              // Stack Segment Selector (SS)
    __int64 rflags;          // RFLAGS register
} COMPLETE_PROCESSOR_STATE;
#pragma pack(pop)



extern void* isr_stub_table[];

__declspec(align(16))
	static idt_entry_t idt[IDT_SIZE];

__declspec(align(16))
    extern idtr_t idtr;

static int vectors[IDT_MAX_DESCRIPTORS];


///  FUNCTIONS

void InterruptCommonHandler(
	__int8 InterruptIndex,   // [0x0, 0xFF
	INTERRUPT_STACK_COMPLETE* StackPointer,      // Pointer to Stack Pointer After Transfer to Handler (Fig 6-9)
	__int8 ErrorCodeAvailable, // 0 if not available
	COMPLETE_PROCESSOR_STATE* ProcessorState // Pointer to a structure which contains trap context (see above trap frame dump example)
);

//__declspec(noreturn) void exception_handler(void);
void idt_set_descriptor(__int8 vector, void* isr, __int8 flags);
void idt_init(void);


#endif // !_INTERRUPTS_H_

