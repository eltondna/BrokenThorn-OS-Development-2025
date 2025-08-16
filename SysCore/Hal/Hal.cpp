#ifndef ARCH_X86
#pragma error "[cpu.h] platform not implimented. Define ARCH_X86 for HAL"
#endif

#include "../Header/Hal.h"
#include "../Kernel/DebugDisplay.h"
#include "cpu.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

int _cdecl hal_initialize(){
    i86_cpu_initialize();
    i86_pic_initialize(0x20, 0x28);
    outportb(0x21, inportb(0x21) & ~1);

    i86_pit_initialize();
    i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
    
    // Debug IDT entry
    i86_pit_is_initialized();
    
    enable(); 
    return 0;
}
int  _cdecl hal_shutdown(){
    i86_cpu_shutdown();
    return 0;
}
void _cdecl geninterrupt(int n){
    #ifdef _MSC_VER
        _asm{
            mov al , byte ptr [n];
            mov byte ptr [genint+1], al
            jmp genint
        genint:
            int 0
        }
    #endif
}


void _cdecl interruptdone(unsigned int intno){

    // DebugGotoXY(0, 10);
    // DebugPrintf("EOI for INT %d\n", intno);
    if (intno > 16)
        return;
    
    if (intno >= 8){
        // Slave handle end of interrupt
        i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, 1);
    }
    // Always send EOI to primary pic (if interrupt is from slave, mark IRQ2 handled)
    i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, 0);

    // DebugGotoXY(0, 12);
    // DebugPrintf("PIC1 IMR: 0x%x", inportb(0x21));

}

//! output sound to speaker (Need further learn)
void _cdecl sound(unsigned frequency){
	//! sets frequency for speaker. frequency of 0 disables speaker
	outportb (0x61, 3 | unsigned char(frequency<<2) );
}

unsigned char _cdecl inportb(unsigned short portid){
    unsigned char result;
    #ifdef _MSC_VER
    _asm {
        mov dx, portid
        in al, dx
        mov result, al
    }
    #endif
    return result;
}

void _cdecl outportb(unsigned short portid, unsigned char value){
    #ifdef _MSC_VER
    _asm{
        mov al, byte ptr [value]
        mov dx, word ptr [portid]
        out dx, al
    }
    #endif
}

void _cdecl enable(){
    #ifdef _MSC_VER
	_asm sti
    #endif
}

void _cdecl disable(){
    #ifdef _MSC_VER
	_asm cli
    #endif
}

void _cdecl setvect(int intno, void (*handler)(void)){
    i86_install_ir(intno, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, 0x8, (I86_IRQ_HANDLER) handler);
}


// Return current Interrupt vector
void * _cdecl getvect(int intno){
    idt_descriptor * desc = i86_get_ir(intno);
    if (!desc) return nullptr;

    // Get the address of the interrupt handler
    uint32_t addr = desc->baseLo | (desc->baseHi << 16);
    // Return Interrupt handler
    I86_IRQ_HANDLER irq = (I86_IRQ_HANDLER) addr;
    return irq;
}


const char*	_cdecl get_cpu_vender (){
    return i86_cpu_get_vendor();
} 

int _cdecl get_tick_count () {
	return i86_pit_get_tickcount();
}


bool get_pit_state(){
    return i86_pit_is_initialized();
}
