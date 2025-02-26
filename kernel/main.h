#ifndef _MAIN_H_
#define _MAIN_H_

#include <intrin.h>


#define true 1
#define false 0
//
// default types
//
typedef unsigned __int8     BYTE, *PBYTE;
typedef unsigned __int16    WORD, *PWORD;
typedef unsigned __int32    DWORD, *PDWORD;
typedef unsigned __int64    QWORD, *PQWORD;
typedef signed __int8       INT8;
typedef signed __int16      INT16;
typedef signed __int32      INT32;
typedef signed __int64      INT64;

//
// exported functions from __init.asm
//
void __cli(void);
void __sti(void);
void __magic(void);         // MAGIC breakpoint into BOCHS (XCHG BX,BX)
void __enableSSE(void);
void __loadIDT(void);
void __haltt(void);
void __print_msg(void);
void __cause_div0(void);
void __cause_pageFault(void);
void __cause_int3(void);
void __isr_pit(void);
void __init_pit(void);
void __send_EOI(void);
void __pit_isr_stub_handler(void);
void __kb_isr_stub_handler(void);
#endif // _MAIN_H_

void
LogSerialAndScreen(
    char* FormatBuffer,
    ...
);