%ifndef _PAGING_INC_
%define _PAGING_INC_


bits 32

; Create a Page directory , and only 2 page table entry for now

;-----------------------------------------------------------------------------;
;                                   Note                                      ;
; The last 20 bits (12-31) is the address of the page / page table in memory  ;
; Through mask off to extract the physical address                            ;
; i.e. 0x9D003 -> then 0x9D003 & 0xFFFFFF000 toi get 0x9D000                  ;
;-----------------------------------------------------------------------------;



%define PAGE_DIR             0x9C000
%define PAGE_TABLE_0         0x9D000
%define PAGE_TABLE_768       0x9E000

%define PAGE_TABLE_ENTRIES   1024
; (Page is Present & Writable)
%define PRIV                 3

;****************************************
;	Enable Paging
;****************************************

EnablePaging:
    pusha 
    mov eax, PAGE_TABLE_0
    mov ebx, 0x0 | PRIV
    mov ecx, PAGE_TABLE_ENTRIES
.loop:
    mov dword[eax], ebx
    add eax, 4
    add ebx, 4096
    loop .loop

    mov eax, PAGE_TABLE_0 | PRIV
    mov dword [PAGE_DIR], eax

    mov eax, PAGE_TABLE_768 | PRIV
    mov dword [PAGE_DIR + 768 * 4], eax 

    ;------------------------------;
    ; Install directory table      ;
    ;------------------------------;

    mov eax, PAGE_DIR
    mov cr3, eax

    ;------------------------------;
    ; Enable Paging                ;
    ;------------------------------;
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    mov eax, PAGE_TABLE_768
    mov ebx, 0x10000 | PRIV
    mov ecx, PAGE_TABLE_ENTRIES

.loop2:
    mov dword [eax], ebx
    add eax, 4
    add ebx, 4096
    loop .loop2

    popa 
    ret 

%endif
