#ifndef _BOOTINFO_H
#define _BOOTINFO_H
#include "stdint.h"

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

struct multiboot_info{
    uint32_t flags;
    uint32_t memoryLo;           // mem_lower (KB)
    uint32_t memoryHi;           // mem_upper (KB)
    uint32_t bootDevice;
    uint32_t cmdLine;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms0, syms1, syms2, syms3;   // 4 dwords
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t bootloader_name;
    uint32_t apm_table;         
    uint32_t vbe_control_info;   
    uint32_t vbe_mode_info;     
    uint16_t vbe_mode;           
    uint16_t vbe_interface_seg;  // u16
    uint16_t vbe_interface_off;  // u16
    uint16_t vbe_interface_len;  // u16
};

#ifdef _MSC_VER
#pragma pack (pop)
#endif

#endif