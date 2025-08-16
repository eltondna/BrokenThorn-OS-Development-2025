
%ifndef __A20_INC_67343546FDCC56AAB872_INCLUDED__
%define __A20_INC_67343546FDCC56AAB872_INCLUDED__

EnableA20_KKbrd_Out:
    cli
    pusha 
    call wait_input
    
    
    ; First disable keyboard
    mov al, 0xAD
    out 0x64, al
    call wait_input

    ; Tell controller to read output port
    mov al, 0xD0
    out 0x64, al
    call wait_output

    ; get output port data and store it
    in al, 0x60
    push eax
    call wait_input
    
    ; Tell controller to write output port
    mov al, 0xD1
    out 0x64, al
    call wait_input

    pop eax
    or eax, 2       ; set bit 1 (enable a20)
    out 0x60, al    ; write out data back to the output port
    call wait_input

    mov al , 0xAE   ; enable keyboard
    out 0x64, al
    call wait_input

    popa 
    sti
    ret


; Wait for input buffer to be clear
wait_input:
    in al, 0x64
    test al, 0x2
    jnz wait_input
    ret


wait_output:
    in al, 0x64
    test al, 0x1
    jz wait_output
    ret

%endif
