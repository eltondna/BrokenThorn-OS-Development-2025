%ifndef _FLOPPY_16
%define _FLOPPY_16


bits 16 

bpbOEM                  db "My OS   "
bpbBytesPerSector       dw 512
bpbSectorPerCluster     db 1
bpbReservedSector       dw 1
bpbNumberOfFATs         db 2
bpbRootEntries          dw 224
bpbTotalSector          dw 2280 
bpbMedia                db 0xF0
bpbSectorPerFAT         dw 9
bpbSectorPerTrack       dw 18
bpbHeadPerCylinder      dw 2
bpbHiddenSector         dd 0
bpbTotalSectorBig       dd 0
bsDriveNumber           db 0
bsUnused                db 0
bsExtBootSignature      db 0x29
bsSerialNumber          dd 0xa1a2a3a4
bsVolumeLabel           db "MOS FLOPPY "
bsFileSystem            db "FAT12   "

datasector              dw 0x0000
cluster                 dw 0x0000



absoluteSector          db 0x00
absoluteHead            db 0x00
absoluteTrack           db 0x00


;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
; ax -> Cluster Number
;************************************************;
ClusterLBA:
    sub     ax, 0x0002                          ; zero base cluster number
    xor     cx, cx
    mov     cl, BYTE [bpbSectorPerCluster]     ; convert byte to word
    mul     cx
    add     ax, WORD [datasector]               ; base data sector
    ret




;************************************************;
; Convert LBA to CHS (AX -> LBA) 
; Absolute Head     : (LBA / Sector Per Track) % number of Head
; Absolute Track    : LBA / (Sector Per Track * number of heads)
; Absolute Sector   : LBA % Sector per Track + 1
;************************************************;
LBACHS:
    pusha  
    
    xor dx, dx
    div word [bpbSectorPerTrack]
    inc dl
    mov byte[absoluteSector], dl
    
    xor dx, dx 
    div word[bpbHeadPerCylinder]

    mov byte [absoluteTrack], al
    mov byte [absoluteHead], dl
    popa 
    ret

;************************************************;
; AX    -> Starting sector 
; ES:BX -> Buffer to read to 
; CX    -> Number of sectors to read
;************************************************;
ReadSectors:
    .main:
        mov di, 0x0005
    .loop:
        push ax
        push bx
        push cx
        call LBACHS                     ; Get absoluteTrack, Sector and Head 
        mov ah, 0x02
        mov al, 0x01                    ; Read 1 sector every time
        mov ch, byte [absoluteTrack]
        mov cl, byte [absoluteSector]
        mov dh, byte [absoluteHead]
        mov dl, byte [bsDriveNumber]
        int 0x13
        jnc .success
        dec di
        pop cx
        pop bx
        pop ax
        jnz .loop
        int 0x18

    .success:
        pop cx
        pop bx
        pop ax
        add bx, word [bpbBytesPerSector]
        inc ax
        loop .main                      ; Rmb loop function use cx as counter
        ret

%endif

