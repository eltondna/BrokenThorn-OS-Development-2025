#ifndef _IDT_H
#define _IDT_H

#ifndef ARCH_X86
#pragma error "[regs.h] platform not implimented. Define ARCH_X86 for HAL"
#endif

#include "../Header/stdint.h"

#define I86_MAX_INTERRUPTS      256
#define I86_IDT_DESC_BIT16      0x06
#define I86_IDT_DESC_BIT32      0x0E
#define I86_IDT_DESC_RING1      0x20
#define I86_IDT_DESC_RING2      0x40
#define I86_IDT_DESC_RING3      0x60
#define I86_IDT_DESC_PRESENT    0x80

//! interrupt handler w/o error code
//! Note Interrupt Handlers are called by the processor, stack set up varies between each iNT
//! so we leave it up to the interrupts' implimentation to handle it and properly return
typedef void (_cdecl *I86_IRQ_HANDLER)(void);

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

struct idt_descriptor {
    uint16_t baseLo;

    uint16_t sel;

    uint8_t reserved;

    uint8_t flag;

    uint16_t baseHi;
};


extern idt_descriptor* i86_get_ir(uint32_t i);

extern int i86_install_ir(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER handler);

extern int i86_idt_initialize(uint16_t codeSel);

#ifdef _MSC_VER
#pragma pack (pop)
#endif


#endif