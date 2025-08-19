#ifndef _MMNGR_VIRTUAL_H
#define _MMNGR_VIRTUAL_H

#include "stdint.h"
#include "vmmngr_pte.h"
#include "vmmngr_pde.h"

typedef uint32_t            virtual_addr;
#define PAGE_TABLE_SIZE     1024
#define PAGE_DIRECTORY_SIZE 1024

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) ((*x) & 0x3FF)

struct ptable {
    pt_entry m_entries[PAGE_TABLE_SIZE];
};
struct pdirectory{
    pd_entry m_entries[PAGE_DIRECTORY_SIZE];
};

extern void MmMapPage(void* phys,void* virt);
extern void vmmngr_initialize();
extern void vmmngr_alloc_page(pt_entry * e);
extern void vmmngr_free_page(pt_entry * e);
extern void vmmngr_switch_directory(pdirectory *);
extern void vmmngr_get_directory();
extern void vmmngr_flush_TLB(virtual_addr addr);
extern uint32_t vmmngr_ptable_virt_to_idx(virtual_addr addr);
extern uint32_t vmmngr_pdirectory_virt_to_idx(virtual_addr addr);
extern void vmmngr_ptable_clear(ptable * p);
extern void vmmngr_pdirectory_clear(pdirectory* dir );
extern pt_entry* vmmngr_ptable_lookup_entry(ptable * p, virtual_addr addr);
extern pd_entry* vmmngr_pdirectory_lookup_entry(pdirectory * p,virtual_addr addr);

#endif