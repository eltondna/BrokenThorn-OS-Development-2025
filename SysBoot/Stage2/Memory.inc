%ifndef _MEMORY_INC
%define _MEMORY_INC

bits 16

;-----------------------------------;
; Memory map entry structure        ;
;-----------------------------------;

struc MemoryMapEntry 
    .baseAddress    resq    1
    .length         resq    1
    .type           resq    1
    .acpi_null      resq    1
endstruc

;-----------------------------------------------------;
;   Get memory map from BIOS                          ;
;   Args : es:di ->Destination buffer for the entries ;
;   Ret  : bp = entry count                           ;
;-----------------------------------------------------;
BiosMemoryMap:
    pushad
    xor ebx, ebx          
    xor bp, bp
.get_entry:
    mov edx, 0x534D4150
    mov eax, 0xe820
    mov ecx, 24
    int 0x15
    jc .done             
    cmp eax, 0x534D4150
    jne .done

    
    mov ecx, [es:di + 8]         
    mov edx, [es:di + 12]        
    test ecx, ecx
    jne .valid
    test edx, edx
    je .skip

.valid:
    inc bp
    add di, 24

.skip:
    test ebx, ebx
    jnz .get_entry

.done:
    mov [0x2000], bp
    popad
    ret

; ; Get Memory size for 64M configuration (32 bit)
; ; return: 
; ;       ax : KB between 1MB and 16MB
; ;       bx : Number of 64 Blocks above 16MB
; ;       ax = -1 and bx = 0 if error
; BiosGetMemorySize64MB_32Bit:
;     push ecx 
;     push edx
;     xor ecx, ecx
;     xor edx, edx 

;     mov ax, 0xe881
;     int 0x15
;     jc .error 
;     cmp ah, 0x86        ; Unsupported function 
;     je .error
;     cmp ah, 0x80        ; Invalid Command  
;     je .error 
;     jcxz .use_ax        ; BIOS may store in ax, test if cx is 0
;     movzx eax, cx          
;     movzx ebx, dx
;     jmp .done

; .use_ax:
;     movzx eax, ax          
;     movzx ebx, bx
; .done:
;     pop edx
;     pop ecx
;     ret

; .error:
;     mov ax, -1 
;     mov bx, 0
;     pop edx 
;     pop ecx
;     ret


BiosGetMemorySize64MB:
	push	ecx
	push	edx
	xor		ecx, ecx
	xor		edx, edx
	mov		ax, 0xe801
	int		0x15	
	jc		.error
	cmp		ah, 0x86		;unsupported function
	je		.error
	cmp		ah, 0x80		;invalid command
	je		.error
	jcxz	.use_ax			;bios may have stored it in ax,bx or cx,dx. test if cx is 0
	mov		ax, cx			;its not, so it should contain mem size; store it
	mov		bx, dx

.use_ax:
	pop		edx				;mem size is in ax and bx already, return it
	pop		ecx
	ret

.error:
	mov		ax, -1
	mov		bx, 0
	pop		edx
	pop		ecx
	ret

%endif