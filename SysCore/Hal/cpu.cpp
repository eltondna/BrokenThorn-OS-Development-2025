#include "cpu.h"
#include "gdt.h"
#include "idt.h"

//! initialize the processors
int i86_cpu_initialize (){
    i86_gdt_initialize();
    i86_idt_initialize(0x08);
    return 0;
}
//! shutdown the processors
void i86_cpu_shutdown (){
    
}

char* i86_cpu_get_vendor(){
    static char vendor[32] = {0};
    // Call cpuid with eax = 0 -> ask vendor id and name
    #ifdef _MSC_VER
    _asm{
        mov eax, 0
        cpuid
        mov dword ptr [vendor], ebx
        mov dword ptr [vendor], edx
        mov dword ptr [vendor], ecx
    }
    #endif
    return vendor;
}
