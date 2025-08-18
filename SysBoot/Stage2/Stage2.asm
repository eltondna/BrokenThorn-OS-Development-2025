bits 16 
; Remember the memory map -- 0x500 through 0x7bff is unsed above the BIOS data area
; We are loaded at 0x500
org 0x500
jmp main


;*******************************************************
;	Preprocessor directives
;*******************************************************
%include "stdio.inc"
%include "gdt.inc"
%include "A20.inc"
%include "Fat12.inc"
%include "common.inc"
%include "Memory.inc"
%include "bootinfo.inc"
;*******************************************************
;	Data Section
;*******************************************************
LoadingMsg db 0x0D, 0x0A, "Searching for Operating System...", 0x00
msgFailure db 0x0D, 0x0A, "*** FATAL: MISSING OR CURRUPT KRNL.SYS. Press Any Key to Reboot", 0x0D, 0x0A, 0x0A, 0x00
BadImage db "*** FATAL: Invalid or corrupt kernel image. Halting system.", 0


boot_info:
istruc multiboot_info
    at multiboot_info.flags            , dd 0
    at multiboot_info.memoryLo         , dd 0
    at multiboot_info.memoryHi         , dd 0
    at multiboot_info.bootDevice       , dd 0
    at multiboot_info.cmdLine          , dd 0
    at multiboot_info.mods_count       , dd 0
    at multiboot_info.mods_addr        , dd 0
    at multiboot_info.syms0            , dd 0
    at multiboot_info.syms1            , dd 0
    at multiboot_info.syms2            , dd 0
    at multiboot_info.syms3            , dd 0      
    at multiboot_info.mmap_length      , dd 0
    at multiboot_info.mmap_addr        , dd 0
    at multiboot_info.drives_length    , dd 0
    at multiboot_info.drives_addr      , dd 0
    at multiboot_info.config_table     , dd 0
    at multiboot_info.bootloader_name  , dd 0
    at multiboot_info.apm_table        , dd 0     
    at multiboot_info.vbe_control_info , dd 0      
    at multiboot_info.vbe_mode_info    , dd 0      
    at multiboot_info.vbe_mode         , dw 0      
    at multiboot_info.vbe_interface_seg, dw 0
    at multiboot_info.vbe_interface_off, dw 0
    at multiboot_info.vbe_interface_len, dw 0
iend

;*******************************************************
;	STAGE 2 ENTRY POINT
;
;		-Store BIOS information
;		-Load Kernel
;		-Install GDT; go into protected mode (pmode) 
;		-Jump to Stage 3
;*******************************************************
main:

;-------------------------------;
;   Setup segments and stack	;
;-------------------------------;
    cli
    xor ax, ax
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov ax, 0x9000
    mov ss, ax
    mov sp, 0xFFFF
    sti
    
    mov [boot_info + multiboot_info.bootDevice], dl

;-------------------------------;
;   Install our GDT		;
;-------------------------------;
    call InstallGDT
;-------------------------------;
;   Enable A20			;
;-------------------------------;
	call EnableA20_KKbrd_Out
    sti
    
    ; Get Available Memory  
    xor eax, eax
    xor ebx, ebx

    ; Get MemoryMap entry
    mov eax, 0x0
    ; mov ds, ax
    mov di, 0x1000
    call BiosMemoryMap

    ; In real mode (like BIOS mode), the 8086/8088 can only address 1MB (20-bit addresses, 
    ; 0x00000–0xFFFFF).

    ;But newer CPUs can address beyond 1MB.

    ;If the A20 line is disabled, memory addresses wrap around at 1MB
    ;(i.e., 0x00100000 acts like 0x00000000).

    ;So, if we write different values to both 0x00000000 and 0x00100000, 
    ;and read back from 0x00000000, we can tell whether A20 is enabled.

    ; mov byte[0x00000000], 0xAA
    ; mov byte[0x00100000], 0xBB
    ; cmp byte [0x00000000], 0xAA  
    mov si , LoadingMsg
    call Puts16
    call LoadRoot	

;-------------------------------;
; Load Kernel			;
;-------------------------------;
    mov ebx, 0
    mov bp, IMAGE_RMODE_BASE
    mov si, ImageName
    call LoadFile
    mov	dword [ImageSize], ecx
    cmp ax, 0

    je enterStage3
    mov	si, msgFailure
    call Puts16
    mov ah, 0
    int     0x16                    ; await keypress
	int     0x19                    ; warm boot computer
	cli				
	hlt

;-----------------------------;
; Enter PMode                 ;
;-----------------------------;
enterStage3:
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_DESC:Stage3



;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************
bits 32

%include "Paging.inc"


Stage3:
    mov ax, DATA_DESC
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x90000
    call ClrScr32
    call EnablePaging


;-------------------------------;
; Copy kernel to 1MB		;
;-------------------------------;
CopyImage:
  	 mov	eax, dword [ImageSize]
  	 movzx	ebx, word [bpbBytesPerSector]
  	 mul	ebx
  	 mov	ebx, 4
  	 div	ebx
   	 cld
   	 mov    esi, IMAGE_RMODE_BASE
   	 mov	edi, IMAGE_PMODE_BASE
   	 mov	ecx, eax
   	 rep	movsd                   ; copy image to its protected mode address
                                    ; rep movsd ( 每次复制 4 字节)


TestImage:
  	  mov    ebx, [IMAGE_PMODE_BASE+60]
  	  add    ebx, IMAGE_PMODE_BASE    ; ebx now points to file sig (PE00)
  	  mov    esi, ebx
  	  mov    edi, ImageSig
  	  cmpsw
  	  je     EXECUTE
  	  mov	ebx, BadImage
  	  call	Puts32
  	  cli
  	  hlt
ImageSig db 'PE'

EXECUTE:

	;---------------------------------------;
	;   Execute Kernel
	;---------------------------------------;

    ; parse the programs header info structures to get its entry point

	add		ebx, 24
	mov		eax, [ebx]			; _IMAGE_FILE_HEADER is 20 bytes + size of sig (4 bytes)
	add		ebx, 20-4			; address of entry point
	mov		ebp, dword [ebx]		; get entry point offset in code section	
	add		ebx, 12				; image base is offset 8 bytes from entry point
	mov		eax, dword [ebx]		; add image base
	add		ebp, eax
	cli

    ; Multiboot spec says eax should be this 
    mov eax, 0x2badb002
    mov ebx, 0          ; <- According to the spec , this should be boot_info
    mov edx, [ImageSize] 
    movzx ecx, word [0x2000] 
    push ecx
    push dword boot_info
	call	ebp               	      ; Execute Kernel

    cli
	hlt

 