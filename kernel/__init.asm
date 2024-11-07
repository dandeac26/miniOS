;;-----------------_DEFINITIONS ONLY_-----------------------
;; IMPORT FUNCTIONS FROM C
%macro IMPORTFROMC 1-*
%rep  %0
    %ifidn __OUTPUT_FORMAT__, win32 ; win32 builds from Visual C decorate C names using _ 
    extern _%1
    %1 equ _%1
    %else
    extern %1
    %endif
%rotate 1 
%endrep
%endmacro


;; EXPORT TO C FUNCTIONS
%macro EXPORT2C 1-*
%rep  %0
    %ifidn __OUTPUT_FORMAT__, win32 ; win32 builds from Visual C decorate C names using _ 
    global _%1
    _%1 equ %1
    %else
    global %1
    %endif
%rotate 1 
%endrep
%endmacro

%define break xchg bx, bx

IMPORTFROMC KernelMain

TOP_OF_STACK                equ 0x200000
KERNEL_BASE_PHYSICAL        equ 0x200000

;;-----------------^DEFINITIONS ONLY^-----------------------

segment .text
[BITS 32]
ASMEntryPoint:
    cli
    MOV     DWORD [0x000B8000], 'O1S1'
%ifidn __OUTPUT_FORMAT__, win32
    MOV     DWORD [0x000B8004], '3121'                  ; 32 bit build marker
%else
    MOV     DWORD [0x000B8004], '6141'                  ; 64 bit build marker
%endif

    MOV     ESP, TOP_OF_STACK                           ; just below the kernel
    
    CALL l_paging_setup
    CALL l_long_mode_setup

    LGDT [GDT64]

    JMP 8:l_long_mode_execution 

    l_end_64bit_execution:

    MOV     EAX, KernelMain     ; after 64bits transition is implemented the kernel must be compiled on x64
    CALL    EAX
    
    break
    CLI
    HLT

;;--------------------------------------------------------


__cli:
    CLI
    RET

__sti:
    STI
    RET

__magic:
    XCHG    BX,BX
    RET
    
__enableSSE:                ;; enable SSE instructions (CR4.OSFXSR = 1)  
    MOV     EAX, CR4
    OR      EAX, 0x00000200
    MOV     CR4, EAX
    RET
    
__loadIDT:
      ; Load the IDT
    lidt [idtr] ; Load the IDT base and limit
    sti        ; Set the interrupt flag

    RET

__haltt:
    hlt
    RET



EXPORT2C ASMEntryPoint, __cli, __sti, __magic, __enableSSE, __loadIDT, __haltt, __print_msg, __cause_div0, __cause_pageFault, __cause_int3

align 0x1000
  PML4_table:
	times 512 dq 0
  PDP_table:
	times 512 dq 0
  PD_table:
	times 512 dq 0
  PT_table:
    times 512 dq 0
  PA_table:
    times 512 dq 0

GDT64:
    .limit  dw  GDT64_Table.end - GDT64_Table - 1
    .base   dd  GDT64_Table

FLAT_DESCRIPTOR_CODE64  equ 0x00AF9A000000FFFF  ; Code: Execute/Read
FLAT_DESCRIPTOR_DATA64  equ 0x00AF92000000FFFF  ; Data: Read/Write
FLAT_DESCRIPTOR_CODE32  equ 0x00CF9A000000FFFF  ; Code: Execute/Read
FLAT_DESCRIPTOR_DATA32  equ 0x00CF92000000FFFF  ; Data: Read/Write
FLAT_DESCRIPTOR_CODE16  equ 0x00009B000000FFFF  ; Code: Execute/Read, accessed
FLAT_DESCRIPTOR_DATA16  equ 0x000093000000FFFF  ; Data: Read/Write, accessed

GDT64_Table:
    .null     dq 0                         ;  0
    .code64   dq FLAT_DESCRIPTOR_CODE64    ;  8
    .data64   dq FLAT_DESCRIPTOR_DATA64    ; 16
    .code32   dq FLAT_DESCRIPTOR_CODE32    ; 24
    .data32   dq FLAT_DESCRIPTOR_DATA32    ; 32
    .code16   dq FLAT_DESCRIPTOR_CODE16    ; 40
    .data16   dq FLAT_DESCRIPTOR_DATA16    ; 48 
    .end:



; Set up paging by linking tables and declaring pages
l_paging_setup:
   
	MOV eax, PDP_table
	OR eax, 0x11 
	MOV [PML4_table], eax
	
	
	MOV eax, PD_table
	OR eax, 0x11  
	MOV [PDP_table], eax
	
	
	MOV eax, PT_table
	OR eax, 0x11 
	MOV [PD_table], eax
	
	
	MOV eax, PA_table
	OR eax, 0x11 
    MOV ecx, PD_table
    ADD ecx, 0x8
	MOV [ecx], eax

    MOV edx, 0;
    MOV ecx, 0 ; 
    MOV esi, PT_table 

    loop_pages:

    MOV eax, edx
	OR eax, 0x11 
	MOV [esi], eax

    ADD edx, 4096
    ADD esi, 8
    INC ecx
    CMP ecx, 1024

    JB loop_pages

    ret

l_long_mode_setup:
    ;set pae bit in cr4
    MOV eax, cr4
    OR eax, 1 << 5
    MOV cr4, eax

    ;cr3 points to pml4 table
	MOV eax, PML4_table
	MOV cr3, eax

    ;set lme bit in ia32_efer
    MOV ecx, 0xC0000080
    RDMSR
    OR eax, 1 << 8
    WRMSR

    ;set pg bit in cr0
    MOV eax, cr0
    OR eax, 1 << 31
    MOV cr0, eax
    
    ret


[BITS 64]

extern idt_init 
extern idtr

l_long_mode_execution:

    ;reload segment reg
    MOV ax, 0
    MOV ss, ax
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    ; print `OKAY` to screen
    ;MOV rax, 0x2f592f412f4b2f4f
    ;MOV qword [0xb8000], rax
    
    ; HANDLE INTERRUPTS
    
    sub rsp, 32 ;allocate shadow space
    call idt_init
    add rsp, 32 ;restore stack p
     
    ; Load the IDT
    lidt [idtr]   
    
    ; Load the IDT base and limit
    sti                  ; Set the interrupt flag

    ;break
    ;mov rax, 10
    ;xor rdx, rdx
    ;xor rbx, rbx
    ;div rbx ; generate div by 0 interrupt
    ;break

    JMP l_end_64bit_execution



;; OSDEV tutorial


global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dq isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1 
%endrep



;; end osdev part

;; lab macros
%macro isr_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push DWORD 1 ; there is an error code
    push  DWORD %1 ; interrupt index
    jmp exception_handler
%endmacro

%macro isr_no_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push  DWORD 0 ; push a dummy error code to have same interrupt frame on no error exceptions
    push  DWORD 0 ; there is no error code
    push  DWORD %1 ; interrupt index
    jmp exception_handler
%endmacro

extern InterruptCommonHandler
;; DEFINE 32 EXCEPTION HANDLERS (STUBS)
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

exception_handler:

    ; interrupt index
    ; has_error_code
    ; rax           0x1FFFF8
    ; rbx  0x1xFFFF0

    ; Save the state of all general-purpose registers
    break
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Save RFLAGS
    pushfq               ; Save RFLAGS onto the stack

    ; Manually save the segment registers to general-purpose registers or memory
    mov rax, cs          ; Save CS in RAX
    push rax             ; Push CS onto the stack

    mov rax, ss          ; Save SS in RAX
    push rax             ; Push SS onto the stack

    mov rax, ds          ; Save DS in RAX
    push rax             ; Push DS onto the stack

    mov rax, es          ; Save ES in RAX
    push rax             ; Push ES onto the stack

    mov r8, rsp
    ; Adjust RSP to reach interrupt_index and error_code
    mov rax, rsp         ; Save the current stack pointer
    add rsp, 160        ; Skip past saved state to reach interrupt_index and error_code
    
    pop rcx              ; Load interrupt_index
    pop rdx               ; Load error_code
    mov r9, rsp
    break

    ; Restore RSP to the original position
    mov rsp, rax         ; Restore stack pointer to its original position

    ; Call the interrupt handler with rcx and r8 set
    call InterruptCommonHandler

    ; Restore segment registers, RFLAGS, and general-purpose registers (reverse order)
    pop rax              ; Retrieve ES
    mov es, ax           ; Restore ES

    pop rax              ; Retrieve DS
    mov ds, ax           ; Restore DS

    pop rax              ; Retrieve SS
    mov ss, ax           ; Restore SS

    pop rax              ; Retrieve CS
    mov cs, ax          ;restore cs

    popfq                ; Restore RFLAGS

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Return to the previous context (if this is an interrupt handler)
    iretq




exception_handler2: 

    pop rcx               ; inter_index   
    pop r8 ; Pop ErrorCodeAvailable into r8 (0 or 1)




    ; Align RSP to 16-byte boundary before the call
    sub rsp, 0x28          ; Allocate shadow space (32 bytes, 0x20) and align (8 extra bytes)

    ; Set up COMPLETE_PROCESSOR_STATE structure in memory
    lea rax, [rsp + 0x28]  ; Calculate address for COMPLETE_PROCESSOR_STATE
    mov [rax], rax         ; Store RAX
    mov [rax + 8], rbx     ; Store RBX
    mov [rax + 16], rcx    ; Store RCX
    mov [rax + 24], rdx    ; Store RDX


    mov rbx, rsp
    add rbx, 0x28


    mov [rax + 32], rbx    ; Store RSP
    mov [rax + 40], rbp    ; Store RBP
    mov [rax + 48], rsi    ; Store RSI
    mov [rax + 56], rdi    ; Store RDI
    mov [rax + 64], r8     ; Store R8
    mov [rax + 72], r9     ; Store R9
    mov [rax + 80], r10    ; Store R10
    mov [rax + 88], r11    ; Store R11
    mov [rax + 96], r12    ; Store R12
    mov [rax + 104], r13   ; Store R13
    mov [rax + 112], r14   ; Store R14
    mov [rax + 120], r15   ; Store R15

    ; Save segment selectors in COMPLETE_PROCESSOR_STATE
    mov ax, cs
    mov [rax + 128], ax    ; Store CS
    mov ax, ss
    mov [rax + 130], ax    ; Store SS
    mov ax, ds
    mov [rax + 132], ax    ; Store DS
    mov ax, es
    mov [rax + 134], ax    ; Store ES

    ; Save RFLAGS
    pushfq
    pop rax                 ; Store RFLAGS temporarily in RAX
    mov [rax + 136], rax    ; Store RFLAGS in COMPLETE_PROCESSOR_STATE

    ; INTERRUPT_STACK_COMPLETE structure in memory
    lea rdx, [rsp + 0x10]  ; Load address for INTERRUPT_STACK_COMPLETE in RDX
    mov [rdx], rsp         ; Set RIP in INTERRUPT_STACK_COMPLETE
    mov [rdx + 8], cs      ; Set CS
    pushfq
    pop rax                 ; Store RFLAGS temporarily in RAX
    mov [rdx + 16], rax     ; Set RFLAGS in INTERRUPT_STACK_COMPLETE
    mov [rdx + 24], rsp    ; Set RSP
    mov [rdx + 32], ss     ; Set SS


    ; Call the C function
    mov r9, rax             ; Pass pointer to COMPLETE_PROCESSOR_STATE

    call InterruptCommonHandler

    ; Clean up stack and return from interrupt
    add rsp, 0x28
    iretq




__print_msg:
    ;print `OKAY` to screen
    MOV rax, 0x2f592f412f4b2f4f
    MOV qword [0xb8000], rax

    ; Write "DIV0" to video memory
    ;mov rax, 0x00444f5649444f00  ; "DIV0" with padding for word alignment
    ;mov [0xb8000], rax

    RET

__cause_div0:
        
    mov rax, 10
    xor rdx, rdx
    xor rbx, rbx
    div rbx ; generate div by 0 interrupt
    
    RET

__cause_pageFault:
; Attempt to access an unmapped memory address
    mov eax, 0xFFFFFFFF  ; Use a high memory address that is likely to cause a page fault
    mov ebx, [eax]      ; Dereference the address to cause a page fault

    ; Exit gracefully (this part will not be reached due to the page fault)
    mov eax, 1          ; syscall: exit
    xor ebx, ebx        ; exit code: 0
    int 0x80            ; invoke syscall
    RET

__cause_int3:
    INT3
    RET