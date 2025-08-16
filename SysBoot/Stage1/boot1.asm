bits 16

org 0

start: jmp main

OEM:                     db "MY OS   " ; OS Name , must be 8 bytes
bytePerSector:           dw 512
sectorPerCluster:        db 1
reservedSector:          dw 1
numberOfFATs:            db 2
rootEntries:             dw 224
totalSectors:            dw 2880       ; Floppy Disk size: 1.4 MB
; Bit 0   (Head)             0: single     | 1: double
; Bit 1   (Sectors per FAT)  0: 9 sectors  | 1: 8 sectors
; Bit 2   (Tracks)           0: 80         | 1: 40 tracks
; Bit 3   (Type)             0: fixed disk | 1: Removable disk
; Bit 4-7 (Reserved: default all 1)          
media:                    db 0xF8  
sectorsPerFAT:            dw 9
headPerCylinder:          dw 2
sectorPerTrack:           dw 18

HiddenSectors: 	DD 0
TotalSectorsBig:     DD 0
DriveNumber: 	        DB 0
Unused: 		DB 0
ExtBootSignature: 	DB 0x29
SerialNumber:	        DD 0xa0a1a2a3
VolumeLabel: 	        DB "MOS FLOPPY "
FileSystem: 	        DB "FAT12   "


Print:
    lodsb       ; mov string from SI to AL 
    or al, al 
    jz Print_done
    mov ah, 0xe
    int 0x10
    jmp Print
Print_done:
    ret


;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;

ReadSectors:
    mov di, 0x0005      ; Five Retries
    .sector_loop:
        push ax
        push bx 
        push cx 
        call LBACHS
        mov ah, 0x02
        mov al, 0x01
        mov ch, BYTE [absoluteTrack]
        mov cl, BYTE [absoluteSector]
        mov dh, BYTE [absoluteHead]
        mov dl, 0               ; Drive number = 0
        int 0x13
        jnc .SUCCESS

        xor ax, ax
        int 0x13                ; Reset the floppy Drive to the first sector
        dec di
        pop cx
        pop bx
        pop ax
        jnz .sector_loop
        int 0x18                ; Tell BIOS Read sector failed

    .SUCCESS:
        mov si, ProgressMsg
        call Print
        pop cx
        pop bx
        pop ax
        add bx, [bytePerSector]
        inc ax
        loop ReadSectors
        ret


;************************************************;
; Convert CHS to LBA
; LBA = (Cluster - 2) * sectors per cluster
;************************************************;
ClusterLBA:
    sub     ax, 0x0002                          ; zero base cluster number
    xor     cx, cx
    mov     cl, BYTE [sectorPerCluster]     ; convert byte to word
    mul     cx
    add     ax, WORD [datasector]               ; base data sector
    ret



;************************************************
; Convert LBA to CHS 
; AX -> LBA address to convert
; absolute sector = (logical sector % sectors per track)
; absolute head   = (logical sector / sectors per track) % number of Head 
; absolute track  = (logical sector / sectors per track * number of Head)
; ***********************************************

LBACHS:
    xor dx, dx
    div word [sectorPerTrack]
    inc dl
    mov BYTE [absoluteSector], dl
    
    xor dx, dx
    div word[headPerCylinder]
    mov byte[absoluteHead], dl
    mov byte[absoluteTrack], al
    ret



;************************************************;
;       BootLoader Entry Point                   ;
;************************************************;

main:

;----------------------------------------------------
; code located at 0000:7C00, adjust segment registers
;----------------------------------------------------

    cli 
    mov ax, 0x07c0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

;----------------------------------------------------
; create stack
;----------------------------------------------------
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFF
    sti

    mov si, LoadingMsg
    call Print

;----------------------------------------------------
; Step 1: Load the Root directory table 
;----------------------------------------------------
    LOAD_ROOT:
        ; compute size of root directory and store in "cx"
        xor cx, cx
        xor dx, dx
        mov ax, 0x0020              ; 32 bytes entry
        mul WORD [rootEntries]
        div WORD [bytePerSector]
        xchg    ax, cx

        ; compute location of root directory and store in "ax"
        mov al, BYTE [numberOfFATs]
        mul WORD [sectorsPerFAT]
        add ax, WORD [reservedSector]
        mov WORD [datasector], ax        ; The location of root directory 
        add WORD [datasector], cx

        ; Read root directory into memory (7C00:0200)
        mov     bx, 0x0200                            ; copy root dir above bootcode
        call    ReadSectors

;----------------------------------------------------
; Step 2: Find Stage 2 first cluster
;----------------------------------------------------
        ; Browse Root diectory 
        mov ah, 0x00
        int 0x16   ; wait for key

        mov cx , [rootEntries]
        mov di, 0x0020
    .find_loop:
        push cx
        mov cx, 0x000B      ; eleven character name 
        mov si, ImageName
        push di
    rep cmpsb
        pop di
        je LOAD_FAT
        pop cx
        add di, 0x0020
        loop .find_loop
        jmp FAILURE
    

;----------------------------------------------------;
; Step 3: Load FAT
;----------------------------------------------------;

    LOAD_FAT:
    ; save starting cluster of boot image

        mov si, CRLFMsg
        call Print
        ; Get the first Cluster (Bytes 26-27)  
        mov dx , [di + 0x001A]
        mov [cluster], dx
    
    ; Get FAT size -> save it in cx
    ; ax: starting address of the sector
    ; bx: memory address to write into 
    ; cx: Size of FAT
        xor ax, ax
        mov al, [numberOfFATs]
        mul WORD [sectorsPerFAT]

        mov cx, ax
        mov ax, [reservedSector]
        mov bx, 0x0200
        ; read FAT into memory (7C00:0200)
        call ReadSectors

        ; read Stage2.sys into memory (0050:0000)
        mov si, CRLFMsg
        call Print

        mov ax, 0x0050 
        mov es, ax
        mov bx, 0x0000
        push bx

;----------------------------------------------------;
; Step 4: Load Stage 2
;----------------------------------------------------;
    LOAD_IMAGE:
        mov ax, [cluster]
        pop bx
        call ClusterLBA
        xor cx, cx
        mov cl, [sectorPerCluster]
        call ReadSectors
        push bx

        ; Compute the next cluster ax : index
        mov ax, [cluster]
        mov cx, ax
        mov dx, ax
        shr dx, 1
        add cx, dx
        mov bx, 0x0200
        add bx, cx
        mov dx, [bx]

        test ax, 0x0001
        jnz .ODD_CLUSTER

    .EVEN_CLUSTER:
        and dx, 0000111111111111b   ; Take low 12 bits
        jmp .DONE

    .ODD_CLUSTER:
        shr dx, 4                   ; Take high 12 bit
    .DONE:
        mov [cluster], dx
        cmp dx, 0x0FF0
        jb LOAD_IMAGE
    
    DONE:
        mov si, CRLFMsg
        call Print
        push 0x0050
        push 0x0000
        retf

    FAILURE:
        mov si, FailureMsg
        call Print
        mov ah, 0x00
        int 0x16        ; Await Keypress
        int 0x19        ; Warm Boot computer
    

absoluteSector db 0x00
absoluteHead   db 0x00
absoluteTrack  db 0x00
; 0x0D \r
; 0x0A \n
LoadingMsg  db 0x0D, 0x0A, "Loading Boot Image ", 0x0D, 0x0A, 0x00
FailureMsg  db 0x0D, 0x0A, "ERROR : Press Any Key to Reboot", 0x0A, 0x00
CRLFMsg     db 0x0D, 0x0A, 0x00
ProgressMsg db ".", 0


datasector      dw 0x0000
cluster  dw 0x0000

ImageName db "KRNLDR  SYS"

TIMES 510-($-$$) DB 0
DW 0xAA55

