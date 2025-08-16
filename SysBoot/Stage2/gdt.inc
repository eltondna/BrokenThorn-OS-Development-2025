%ifndef __GDT_INC_67343546FDCC56AAB872_INCLUDED__
%define __GDT_INC_67343546FDCC56AAB872_INCLUDED__

bits 16 

;*******************************************
; InstallGDT()
;       - Install out GDT
;*******************************************
InstallGDT:
    cli 
    pusha
    lgdt [toc]
    popa
    sti 
    ret

;*******************************************
; Global Descriptor Table
;*******************************************
gdt_data:
    dd 0    ; null descriptor
    dd 0

; gdt code descriptor
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b     ; Access Byte
    db 11001111b
    db 0

; gdt data descriptor
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b     ; Access Byte
    db 11001111b     ; granularity
    db 0            ; base high 

end_of_gdt:
toc:    
    dw end_of_gdt - gdt_data - 1
    dd gdt_data

%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10

%endif