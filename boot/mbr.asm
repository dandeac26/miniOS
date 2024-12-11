%define break xchg bx, bx  ; magic breakpoint in bochs
 
[bits 16]                   
[org 7C00h]                ; because the MBR is loaded at 7C00h and runs in 16-bit Real Mode            
BootMain:                  
    ;break                   
    xor    ax,    ax       ; start setting up a context
    mov    ds,    ax      
    mov    es,    ax      
    mov    gs,    ax      
    mov    ss,    ax      
    mov    fs,    ax      
 
    mov    sp,    BootMain ; prepare a stack - the memory from 7BFFh down to 500h is unused - see http://www.brokenthorn.com/Resources/OSDev7.html
 
    mov    ah,    0       
    int    13h             ; reset the boot drive
    
    mov    ah,    02h      ; parameters for calling int13 (ReadSectors)
    mov    al,    9        ; read 9 sectors (hardcoded space for SSL in the floppy)
    mov    ch,    00h     
    mov    cl,    02h      ; starting from sector 2 - skip sector 1 (the MBR)
    mov    dh,    00h     
    mov    bx,    0x7E00   ; memory from 0x7E00 - 0x9FFFF is unused
    int    13h             
    jnc    .success        
  
    cli                    ; we should reset drive and retry, but we hlt
    hlt                     
 
.success:   
	
	;call do_e820

	;break

    jmp    00:0x7E00       ; start executing the Second Stage Loader (dl MUST contain the boot drive id)

;mmap_ent equ 0x9000             ; the number of entries will be stored at 0x8000

;do_e820:
;	
;    mov di, 0x9004          ; Set di to 0x8004. Otherwise this code will get stuck in `int 0x15` after some entries are fetched 
;	xor ebx, ebx		; ebx must be 0 to start
;	xor bp, bp		; keep an entry count in bp
;	mov edx, 0x0534D4150	; Place "SMAP" into edx
;	mov eax, 0xe820
;	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
;	mov ecx, 24		; ask for 24 bytes
;	
;	int 0x15
;	jc short .failed	; carry set on first call means "unsupported function"
;	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
;	cmp eax, edx		; on success, eax must have been reset to "SMAP"
;	jne short .failed
;	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
;	je short .failed
;	jmp short .jmpin
;.e820lp:
;
;	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
;	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
;	mov ecx, 24		; ask for 24 bytes again
;	int 0x15
;	jc short .e820f		; carry set means "end of list already reached"
;	mov edx, 0x0534D4150	; repair potentially trashed register
;.jmpin:
;	
;	jcxz .skipent		; skip any 0 length entries
;	cmp cl, 20		; got a 24 byte ACPI 3.X response?
;	jbe short .notext
;	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
;	je short .skipent
;.notext:
;	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
;	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
;	jz .skipent		; if length uint64_t is 0, skip entry
;	inc bp			; got a good entry: ++count, move to next storage spot
;	add di, 24
;.skipent:
;	test ebx, ebx		; if ebx resets to 0, list is complete
;	jne short .e820lp
;.e820f:
;
;	mov [es:mmap_ent], bp	; store the entry count
;	clc			; there is "jc" on end of list to this point, so the carry must be cleared
;	ret
;.failed:
;	
;	stc			; "function unsupported" error exit
;	ret 

	;break
times 510 - ($-$$) db 'B'  ; fill the rest (to 510 bytes) of the sector with padding (we chose B letter from Boot)
db 0x55, 0xAA              ; define the signature of a bootable sector (+2 bytes -> 512)
;break