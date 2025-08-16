#include "exception.h"
#include "Hal.h"
#include "stdint.h"



extern void _cdecl kernel_panic(const char* fmt, ...);
#ifdef _MSC_VER
#define intstart()\
        _asm cli \
        _asm sub ebp, 4
#else
#define intstart()
#endif

// warning C4100: unreferenced formal parameter
#pragma warning (disable:4100)

//warning C4731: frame pointer register 'ebp' modified by inline assembly code
#pragma warning (disable:4731)


void _cdecl divide_by_zero(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Divide by 0 at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl single_step_trap(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Single step trap at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl nmi_trap(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("NMI trap at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl breakpoint_trap(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Break Point trap at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}

void _cdecl overflow_trap(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Overflow trap at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl bounds_check_fault(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Bound check fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl invalid_opcode_fault(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Invalid Opcode fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl no_device_fault(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Device Not found fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}


void _cdecl double_fault_abort(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    kernel_panic("Double fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl invalid_tss_fault(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    kernel_panic("Invalid TSS at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl no_segment_fault(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    kernel_panic("Invalid Segment fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);

}
void _cdecl stack_fault(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    kernel_panic("Stack fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl general_protection_fault(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    kernel_panic("General Protection fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl page_fault(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    int faultAddress = 0;
    _asm{
        mov eax, cr2
        mov [faultAddress], eax
    }

    kernel_panic("Page fault at 0x%x:0x%x referenced memory at 0x%x", cs, eip, faultAddress);
    for(;;);
}


void _cdecl fpu_fault(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("FPU fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl alignment_check_fault(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err){
    intstart();
    kernel_panic("Alignment Check fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl machine_check_fault(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("Machine Check fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}
void _cdecl simd_fpu_fault(uint32_t eip, uint32_t cs, uint32_t eflags){
    intstart();
    kernel_panic("FPU SIMD fault at physical address [0x%x:0x%x] EFLAGs[0x%x]", cs, eip, eflags);
    for(;;);
}



