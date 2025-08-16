%ifndef _FAT_12
%define _FAT_12


bits 16
%include "Floppy16.inc"

%define ROOT_OFFSET 0x7e00
%define FAT_SEG     0x7c0
%define ROOT_SEG    0x7e0


; *******************************************************
; LoadRoot ()                                          *
; 	- Load Root Directory Table to 0x7e00              *
; *******************************************************
; 1. Get the size of root directory (Number of sectors)*
; 2. Get the starting address of the root directory    *
; 3. assign ROOT_SEG to es:bx                          *
; 4. Call ReadSector                                   *
; *******************************************************

LoadRoot:
    pusha
    push es
    ; 1. the size of root directory -> cx
    mov ax, word [bpbRootEntries]
    mov bx, 0x20
    mul bx
    div word [bpbBytesPerSector]
    mov cx, ax

    ; 2. starting address of the root directory -> ax
    mov ax, word [bpbSectorPerFAT]
    mov bl, byte[bpbNumberOfFATs]
    mul bx
    add ax, word [bpbReservedSector]
    mov word [datasector], ax
    add word [datasector], cx


    ; 3. Read Root directory into 0x7e00
    push word ROOT_SEG
    pop es
    xor bx, bx 
    call ReadSectors
    pop es
    popa 
    ret

;*******************************************
; LoadFAT ()
;	- Loads FAT table to 0x7c00
;
;	Parm/ ES:DI => Root Directory Table
;*******************************************
LoadFAT:
    pusha 
    push es

    xor ax, ax
    mov al, byte[bpbNumberOfFATs]
    mul word[bpbSectorPerFAT]
    mov cx, ax

    mov ax, word[bpbReservedSector]
    
    push word FAT_SEG
    pop es
    xor bx, bx
    call ReadSectors
    pop es
    popa 
    ret

;*******************************************
; FindFile ()
;	- Search for filename in root table
;
; parm/ DS:SI => File name
; ret/ AX => File index number in directory table. -1 if error
;*******************************************
; FindFile:
; 	push	cx						; store registers
; 	push	dx
; 	push	bx
;     mov bx, si
;     mov cx , word[bpbRootEntries]
;     mov di, ROOT_OFFSET
;     cld

; .loop:
;     push cx
;     mov cx, 11
;     mov si, bx
;     push di
;     rep cmpsb
;     pop di
;     je .found
;     pop cx
;     add di, 32
;     loop .loop

; .not_found:
; 	pop	bx					
; 	pop	dx
; 	pop	cx
;     mov ax , -1
;     ret

; .found:
;     pop ax                
; 	pop	bx	
; 	pop	dx
; 	pop	cx
;     ret

FindFile:

	push	cx						; store registers
	push	dx
	push	bx
	mov	bx, si						; copy filename for later

     ; browse root directory for binary image

	mov     cx, WORD [bpbRootEntries]			; load loop counter
	mov     di, ROOT_OFFSET						; locate first root entry at 1 MB mark
	cld							; clear direction flag

.LOOP:
	push    cx
	mov     cx, 11					; eleven character name. Image name is in SI
	mov	si, bx						; image name is in BX
 	push    di
     rep  cmpsb							; test for entry match
	pop     di
	je      .Found
	pop     cx
	add     di, 32					; queue next directory entry
	loop    .LOOP

.NotFound:
	pop	bx						; restore registers and return
	pop	dx
	pop	cx
	mov	ax, -1						; set error code
	ret

.Found:
	pop	ax						; return value into AX contains entry of file
	pop	bx						; restore registers and return
	pop	dx
	pop	cx
	ret



; *******************************************
; LoadFile ()
; 	- Load file
; parm/ ES:SI => File to load
; parm/ EBX:BP => Buffer to load file to
; ret/ AX => -1 on error, 0 on success
; ret/ CX => number of sectors read
; *******************************************
LoadFile:
    xor ecx, ecx
    push ecx
    .find_file:
        push bx
        push bp

        call FindFile
        cmp ax, -1
        jne .load_image_pre
        pop bp
        pop bx
        mov ax , -1
        ret

    ; ax should contain the index of file in root entry
    .load_image_pre:
        sub	edi, ROOT_OFFSET

        push word ROOT_SEG
        pop es
        mov dx, word[es:di + 0x001A]
        mov word[cluster], dx
        pop bx
        pop es

        push bx
        push es
        call LoadFAT
        
    .load_image:
        mov ax, WORD[cluster]
        pop es
        pop bx
        call ClusterLBA
        xor cx, cx
        mov cl , BYTE [bpbSectorPerCluster]
        call ReadSectors
        pop ecx
        inc ecx

        push ecx
        push bx
        push es
        mov ax, FAT_SEG
        mov es, ax
        xor bx, bx

        ; Get next cluster 
        mov ax, WORD [cluster]
        mov cx, ax
        mov dx, ax
        shr dx, 0x0001
        add cx, dx

        add bx, cx
        mov dx, WORD [es:bx]
        test ax, 0x0001
        jnz .ODD_CLUSTER        ; Cluster 4 -> 5th cluster , Cluster 10, -> 11th cluster

.EVEN_CLUSTER:
    and dx, 0000111111111111b
    jmp .done

.ODD_CLUSTER:
    shr dx, 0x0004

.done:
    mov WORD [cluster], dx
    cmp dx, 0x0ff0
    jb .load_image
.success:
    pop es
    pop bx
    pop ecx
    xor ax, ax
    ret



%endif



