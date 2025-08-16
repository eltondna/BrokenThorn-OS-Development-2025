#ifndef _REGS_H_INCLUDED
#define _REGS_H_INCLUDED


#ifndef ARCH_X86
#pragma error "[regs.h] platform not implimented. Define ARCH_X86 for HAL"
#endif


#include "../Header/stdint.h"

struct _R32BIT
{
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eflags;
    uint8_t cflag;
};

struct _R16BIT
{
    uint16_t ax, bx, cx, dx, si, di, bp, sp, flags;
    uint8_t cflag;
};

struct _R8BIT {
    uint8_t al, ah, bl, bh, cl, ch, dl, dh;
};

//! 16 bit registers expressed in 32 bit registers
struct _R16BIT32 {
    uint16_t ax, axh, bx, bxh, cx, cxh, dx, dxh;
	uint16_t si, di, bp, sp, es, cs, ss, ds, flags;
	uint8_t cflags;
};

//! 8 bit registers expressed in 32 bit registers
struct _R8BIT32 {
    uint8_t al, ah; uint16_t axh; 
	uint8_t bl, bh; uint16_t bxh; 
	uint8_t cl, ch; uint16_t cxh; 
	uint8_t dl, dh; uint16_t dxh; 
};

union _INTR16{
    struct _R16BIT x;
    struct _R8BIT  h;
};

union _INTR32{
    struct _R32BIT      x;
    struct _R16BIT32    l;
    struct _R8BIT32     h;
};

#endif