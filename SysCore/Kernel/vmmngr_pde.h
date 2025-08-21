#ifndef _VMMNGR_PDE_H
#define _VMMNGR_PDE_H
#include "stdint.h"
#include "mmngr_phys.h"

enum PAGE_PGE_FLAG{
    I86_PDE_PRESENT         = 1,
    I86_PDE_WRITABLE        = 2,
    I86_PDE_USER            = 4,
    I86_PDE_PWT             = 8,
    I86_PDE_PCD             = 16,
    I86_PDE_ACCESSED        = 32,
    I86_PDE_DIRTY           = 64,
    I86_PDE_4MB             = 128,
    I86_PDE_CPU_GLOBAL      = 256,
    I86_PDE_LV4_GLOBAL      = 512,
    I86_PDE_FRAME           = 0xFFFFF000 
};

typedef uint32_t pd_entry;
extern void pd_entry_set_attrib(pd_entry* e, uint32_t attrib);
extern void pd_entry_del_attrib(pd_entry* e, uint32_t attrib);
extern void pd_entry_set_frame(pd_entry* e, physical_addr addr);
extern bool pd_entry_is_present(pd_entry e);
extern bool pd_entry_is_user(pd_entry e);
extern bool pd_entry_is_4mb (pd_entry e);
extern bool pd_entry_is_writable(pd_entry e);
extern physical_addr pd_entry_pfn(pd_entry e);
extern void pd_entry_enable_global(pd_entry e);

#endif