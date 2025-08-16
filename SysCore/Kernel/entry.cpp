#include "../Header/boot_info.h"
#include "stdint.h"
extern int _cdecl main (multiboot_info* bootinfo, uint32_t memorymapEntryCount);
extern void _cdecl InitializeConstructors();
extern void _cdecl Exit ();

void _cdecl kernel_entry(multiboot_info* bootinfo, uint32_t memorymapEntryCount){
    #ifdef ARCH_X86
        _asm{
            cli 
            mov ax, 10h
            mov ds, ax
            mov es, ax
            mov fs, ax
            mov gs, ax
        }
    #endif

    //! Execute global constructors
	InitializeConstructors();
    //!	Call kernel entry point
    main(bootinfo, memorymapEntryCount);
    //! Cleanup all dynamic dtors
    Exit();
    #ifdef ARCH_X86
	_asm {
        cli
        hlt
    }
    #endif
	for (;;);

}