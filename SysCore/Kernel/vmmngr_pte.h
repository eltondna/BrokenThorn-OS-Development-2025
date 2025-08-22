#ifndef _VMMNGR_PTE_H
#define _VMMNGR_PTE_H

#include "stdint.h"
#include "mmngr_phys.h"

enum PAGE_PTE_FLAGS{
    I86_PTE_PRESENT         = 1,
    I86_PTE_WRITABLE        = 2,
    I86_PTE_USER            = 4,
    I86_PTE_WRITETHROUGH    = 8,
    I86_PTE_NOT_CACHEABLE   = 16,
    I86_PTE_ACCESSED        = 32,
    I86_PTE_DIRTY           = 64,
    // Bit 7 : Page Attribute table : Use with WRITETHROUGH and NOT_CACHEABLE
    I86_PTE_PAT             = 128,
    // Bit 8 : Mark certain Page global, so TLB Flush wont flush this page
    I86_PTE_CPU_GLOBAL      = 256,
    // Bit 9 - 11 use for our own
    I86_PTE_LV4_GLOBAL      = 512,
    // Mask should be this ? high 20 bit address , low 12 bit attrib bit
    I86_PTE_FRAME           = 0xFFFFF000 
};

typedef uint32_t pt_entry;

extern void          pt_entry_set_attrib(pt_entry * e, uint32_t attrib);
extern void          pt_entry_delete_attrib(pt_entry * e, uint32_t attrib);
extern void          pt_entry_set_frame(pt_entry * e, physical_addr addr);
extern bool          pt_entry_is_present(pt_entry e);
extern bool          pt_entry_is_writable(pt_entry e);
extern physical_addr pt_entry_pfn(pt_entry e); // Obtain frame address

#endif