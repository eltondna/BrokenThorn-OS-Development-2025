#ifndef _CPU_H_INCLUDED
#define _CPU_H_INCLUDED


#ifndef ARCH_X86
#pragma error "[cpu.h] platform not implimented. Define ARCH_X86 for HAL"
#endif


#include "../Header/stdint.h"
#include "regs.h"

//! initialize the processors
extern int i86_cpu_initialize ();
//! shutdown the processors
extern void i86_cpu_shutdown ();
// ! Get CPU vendor
extern char* i86_cpu_get_vendor();

#endif