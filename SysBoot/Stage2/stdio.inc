;*************************************************
;	stdio.inc
;		-Input/Output routines
;
;	OS Development Series
;*************************************************
 
%ifndef __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
%define __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
 
;************************************************;
;	Puts16 ()
;		-Prints a null terminated string
;	DS=>SI: 0 terminated string
;************************************************;
 
bits	16
 
Puts16:
		pusha				; save registers
.Loop1:
		lodsb				; load next byte from string from SI to AL
		or	al, al			; Does AL=0?
		jz	Puts16Done		; Yep, null terminator found-bail out
		mov	ah, 0eh			; Nope-Print the character
		int	10h			; invoke BIOS
		jmp	.Loop1			; Repeat until null terminator found
Puts16Done:
		popa				; restore registers
		ret				; we are done, so return
 


bits 32 

%define VIDMEM 	0xB8000
%define COLS 	80
%define ROWS 	25
%define CHAR_ATTR	63

_CURX db 0
_CURY db 0


;**************************************************;
;	Putch32 ()
;		- Prints a character to screen
;	BL => Character to print
;**************************************************;
Putch32:
	pusha 
	mov edi, VIDMEM
	
	
	; currentPos = x + y * COLS!
	xor eax, eax
	mov ecx, COLS * 2
	mov al , byte[_CURY]
	mul ecx
	push eax


	mov al , byte [_CURX]
	mov cl , 2
	mul cl
	pop ecx
	add eax, ecx

	xor ecx, ecx
	add edi, eax	; add it to the base address

	; New Line Character ?
	cmp bl, 0xA
	je .Row

	mov dl, bl			; Low Byte : Character  
	mov dh, CHAR_ATTR	; High Byte: Attribute
	mov [edi], dx


	;-------------------------------;
	;   Update next position        ;
	;-------------------------------;
	inc byte [_CURX]
	jmp .done


.Row:
	mov byte [_CURX], 0
	inc byte [_CURY]

.done:
	popa
	ret

;**************************************************;
;	Puts32 ()
;		- Prints a null terminated string
;	parm\ EBX = address of string to print
;**************************************************;
Puts32:
	pusha 
	push ebx 
	pop edi

.loop:
	mov bl, byte[edi]
	cmp bl, 0
	je .done

	call Putch32
	inc edi
	jmp .loop

.done:

	;-------------------------------;
	;   Update hardware cursor      ;
	;-------------------------------;

	; Its more efficiant to update the cursor after displaying
	; the complete string because direct VGA is slow
	mov bh, byte [_CURY]
	mov bl, byte [_CURX]
	call MovCur

	popa 
	ret

;**************************************************;
;	MoveCur ()
;		- Update hardware cursor
;	parm/ bh = Y pos
;	parm/ bl = x pos
;**************************************************;
MovCur:
	pusha

	;-------------------------------;
	;   Get current position        ;
	;-------------------------------;

	; Here, _CurX and _CurY are relitave to the current position on screen, not in memory.
	; That is, we don't need to worry about the byte alignment we do when displaying characters,
	; so just follow the forumla: location = _CurX + _CurY * COLS


	xor eax, eax
	mov ecx, COLS
	mov al, bh
	mul ecx
	add al, bl
	mov ebx, eax

	;--------------------------------------;
	;   Set low byte index to VGA register ;
	;--------------------------------------;
	; CRTC has 2 registers : index register and data register
	; We need to first tell index register , where we want to write
	; Then tell data register, what we want to write

;Analogy:
	; Think of the CRTC like a bank of 256 lockers (internal registers), but:
	; You can only access one locker at a time
	; First, you say: "I want locker #14" (write to Index Register)
	; Then, you say: "Here's the value for locker #14" (write to Data Register)


	; Write low byte
	mov al, 0x0f 	
	mov dx, 0x3d4	; index reg
	out dx, al	

	mov al, bl
	mov dx, 0x3d5	; data reg
	out dx, al


	; Write high byte
	mov al, 0x0e 
	mov dx, 0x3d4
	out dx, al

	mov al, bh
	mov dx, 0x3d5
	out dx, al

	popa 
	ret


ClrScr32:
	pusha
	mov ax, COLS
	mov cx, ROWS
	mul cx
	mov cx, ax	; cx has the loop counter
	
	mov ah, CHAR_ATTR
	mov al, ' '
	mov edi, VIDMEM
	xor dx, dx

.loop:
	cmp dx, cx
	je .done
	mov [edi], ax
	add edi, 2

	inc dx
	jmp .loop

.done:
    mov byte [_CURX], 0
    mov byte [_CURY], 0
	popa 
	ret

;**************************************************;
;	GotoXY ()
;		- Set current X/Y location
;	parm\	AL=X position
;	parm\	AH=Y position
;**************************************************;

GotoXY:
	pusha
	mov [_CURX], al
	mov [_CURY], ah
	popa 
	ret


%endif ;__STDIO_INC_67343546FDCC56AAB872_INCLUDED__