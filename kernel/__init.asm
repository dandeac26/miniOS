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

    LGDT [__GDT64]

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
    
EXPORT2C ASMEntryPoint, __cli, __sti, __magic, __enableSSE

align 0x1000
__PML4_table:
	times 512 dq 0
__PDP_table:
	times 512 dq 0
__PD_table:
	times 512 dq 0
__PT_table:
    times 512 dq 0
__PA_table:
    times 512 dq 0

__GDT64:
    .limit  dw  __GDT64_Table.end - __GDT64_Table - 1
    .base   dd  __GDT64_Table

FLAT_DESCRIPTOR_CODE64  equ 0x00AF9A000000FFFF  ; Code: Execute/Read
FLAT_DESCRIPTOR_DATA64  equ 0x00AF92000000FFFF  ; Data: Read/Write
FLAT_DESCRIPTOR_CODE32  equ 0x00CF9A000000FFFF  ; Code: Execute/Read
FLAT_DESCRIPTOR_DATA32  equ 0x00CF92000000FFFF  ; Data: Read/Write
FLAT_DESCRIPTOR_CODE16  equ 0x00009B000000FFFF  ; Code: Execute/Read, accessed
FLAT_DESCRIPTOR_DATA16  equ 0x000093000000FFFF  ; Data: Read/Write, accessed

__GDT64_Table:
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
   
	MOV eax, __PDP_table
	OR eax, 0x11 
	MOV [__PML4_table], eax
	
	
	MOV eax, __PD_table
	OR eax, 0x11  
	MOV [__PDP_table], eax
	
	
	MOV eax, __PT_table
	OR eax, 0x11 
	MOV [__PD_table], eax
	
	
	MOV eax, __PA_table
	OR eax, 0x11 
    MOV ecx, __PD_table
    ADD ecx, 0x8
	MOV [ecx], eax

    MOV edx, 0;
    MOV ecx, 0 ; 
    MOV esi, __PT_table 

    loop_pages:

    MOV eax, edx
	OR eax, 0x11 
	MOV [esi], eax

    ADD edx, 4096
    ADD esi, 8
    INC ecx
    CMP ecx, 1024

    JL loop_pages

    ret

l_long_mode_setup:
    ;set pae bit in cr4
    MOV eax, cr4
    OR eax, 1 << 5
    MOV cr4, eax

    ;cr3 points to pml4 table
	MOV eax, __PML4_table
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
l_long_mode_execution:

    ;reload segment reg
    MOV ax, 0
    MOV ss, ax
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    ; print `OKAY` to screen
    MOV rax, 0x2f592f412f4b2f4f
    MOV qword [0xb8000], rax
    
    JMP l_end_64bit_execution
