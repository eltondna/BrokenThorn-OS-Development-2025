#ifndef _MMNGR_PHYS_H
#define _MMNGR_PHYS_H

#include "stdint.h"
typedef uint32_t physical_addr;

// ! Initialize the physical memory manager
extern void pmmngr_init(size_t, physical_addr);

// ! Initialize a physical memory region for use
extern void pmmngr_init_region(size_t, physical_addr);

// ! Deinitialize a physical memory region
extern void pmmngr_deinit_region(size_t, physical_addr);

// ! Allocate a single memory block (4KB) 
extern void* pmmngr_alloc_block();

// ! Release a single memory block  (4KB)
extern void pmmngr_free_block(void* p);

// ! Amount of physical memory the manager is set to use
extern size_t pmmngr_get_memory_size();

// ! Allocate blocks of memory      (> 4KB)
extern void* pmmngr_alloc_regions(size_t size);

// ! Deallocate blocks of memory    (> 4KB)
extern void pmmngr_free_regions(void* p, size_t size);

// ! Return allocated memory blocks
extern uint32_t pmmngr_get_use_block_count();

// ! Return free memory blocks
extern uint32_t pmmngr_get_free_block_count ();

// ! Return total number of memory blocks
extern uint32_t pmmngr_get_total_block_count();

// ! Returns default memory block size in bytes
extern uint32_t pmmngr_get_block_size ();


/* Paging Setting */

// // ！ Enable or disable Paging 
extern void pmmngr_paging_table(bool);

// // ! Test Paging is enabled
extern bool pmmngr_is_paging();

// // ! Loads the page directory base register (PDBR)
extern void pmmngr_load_PDBR();

// // ! Get PDBR physical address
extern physical_addr pmmngr_get_PDBR();

#endif