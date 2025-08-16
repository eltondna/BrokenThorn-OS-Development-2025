#ifndef _GDT_H_INCLUDED
#define _GDT_H_INCLUDED

#ifndef ARCH_X86
#pragma error "[regs.h] platform not implimented. Define ARCH_X86 for HAL"
#endif



#include "../Header/stdint.h"


#define     MAX_DESCRIPTOR              3
#define     I86_GDT_DESC_ACCESS         0x0001
#define     I86_GDT_DESC_READWRITE      0x0002
#define     I86_GDT_DESC_EXPANSION      0x0004
#define     I86_GDT_DESC_EXEC_CODE      0x0008
#define     I86_GDT_DESC_CODEDATA       0x0010      /* Code or Data descriptor */
#define     I86_GDT_DESC_DPL            0x0060      /* Ring 3 Priviledge level */
#define     I86_GDT_DESC_MEMORY         0x0080      /* "In Memory" Bit*/


#define     I86_GDT_GRAND_LIMITHI_MASK  0x0f
#define     I86_GDT_GRAND_OS            0x10
#define     I86_GDT_GRAND_32BIT         0x40
#define     I86_GDT_GRAND_4K            0x80



#ifdef _MSC_VER
// Struct defined is packed with each other
// If not defined, usually 4 byte alignment between data
#pragma pack (push, 1)
#endif

struct gdt_descriptor{
    uint16_t limit;
    uint16_t baseLo;
    uint8_t  baseMid;
    uint8_t  flags;
    uint8_t  grand;
    uint8_t  baseHi;
};


#ifdef _MSC_VER
#pragma pack (pop)
#endif

extern void gdt_set_descriptor(uint32_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t grand);

extern gdt_descriptor* i86_gdt_get_descriptor(int i);

extern int i86_gdt_initialize();


#endif