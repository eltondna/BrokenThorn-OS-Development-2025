#include "string.h"
#include "mmngr_virtual.h"
#include "mmngr_phys.h"

#define PTABLE_ADDR_SPACE_SIZE 0x400000
#define DTABLE_ADDR_SPACE_SIZE 0x100000000
#define PAGE_SIZE              4096

// Current Page Directory address
pdirectory * _cur_directory = 0;
// Current Page Base Address
physical_addr _cur_pdbr     = 0;

inline pt_entry* vmmngr_ptable_lookup_entry(ptable * p, virtual_addr addr){
    if (p){
        return &(p->m_entries[PAGE_TABLE_INDEX(addr)]);
    }
    return 0;
}

inline pd_entry* vmmngr_pdirectory_lookup_entry(pdirectory * p,virtual_addr addr){
    if (p){
        return & (p->m_entries[PAGE_DIRECTORY_INDEX(addr)]); 
    }
    return 0;
}

inline bool vmmngr_switch_directory(pdirectory *dir){
    if (!dir)
        return false;
    _cur_directory = dir;
    pmmngr_load_PDBR(_cur_pdbr);
    return true;

}

void vmmngr_flush_TLB(virtual_addr addr){
    #ifdef _MSC_VER
    _asm {
        cli 
        invlpg addr
        sti
    }
    #endif
}

pdirectory* vmmngr_get_directory(){
    return _cur_directory;
}

bool vmmngr_alloc_page(pt_entry * e){
    // Allocate a free physical frame

    void * p = pmmngr_alloc_block();
    if (!p)
        return false;
    // Map it into the page 
    pt_entry_set_frame(e,(physical_addr) p);
    pt_entry_set_attrib(e, I86_PTE_PRESENT);
    
    // Doesn't set WRITE FLAG for now
    return true;
    
}

void vmmngr_free_page(pt_entry * e){
    void * p = (void *)pt_entry_pfn(*e);
    if (p)
        pmmngr_free_block(p);
    pt_entry_delete_attrib(e, I86_PTE_PRESENT);
    
}

void vmmngr_map_page(void* phys,void* virt){
    // ! Get Page directory 
    pdirectory * pageDirectory = vmmngr_get_directory();
    // ! Get Page Table entry from the Page Directory 
    pd_entry   * e = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];
    
    // Page Table Entry not present
    if ( (*e & I86_PDE_PRESENT) != I86_PDE_PRESENT){
        // Allocate space for page table
        ptable * table = (ptable *) pmmngr_alloc_block();
        if (!table)
            return;
        
        memset(table, 0 ,sizeof(ptable));
        // Create Page Directory entry and set it to point to the new created Page Table
        pd_entry * entry = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t) virt)];
        
        pd_entry_set_attrib(entry, I86_PDE_PRESENT);
        pd_entry_set_attrib(entry, I86_PDE_WRITABLE);
        pd_entry_set_frame(entry, (physical_addr) table);
    }

    // ! Get Table
    ptable * table = (ptable*) PAGE_GET_PHYSICAL_ADDRESS(e);

    // ! Get Page
    pt_entry * pageTableEntry = (pt_entry*) &table->m_entries[PAGE_TABLE_INDEX((uint32_t) virt)];

    // ! Set the Real Physical Address to the page table entry
    pt_entry_set_frame(pageTableEntry, (physical_addr) phys);
    pt_entry_set_attrib(pageTableEntry, I86_PTE_PRESENT);

}

void vmmngr_initialize(){
    // ! 0-4MB Page Table
    ptable * table = (ptable*) pmmngr_alloc_block();
    if (!table)
        return;
    // ! 3GB page table
        ptable* table2 = (ptable*) pmmngr_alloc_block();
    if (!table2)
      return;
    
    memset(table, 0, sizeof(ptable));
    int i;
    int frame = 0x0;
    int virt = 0x00000000;

    // First 4mb is identity Map
    while (i < 1024){
        // Create a new page and set attribute and the physical address
        pt_entry page = 0;
        pt_entry_set_attrib(&page, I86_PDE_PRESENT);
        pt_entry_set_frame(&page, frame);

        // Set it to the page table entry
        table->m_entries[i] = page;

        ++i;
        frame += 4096;
        virt += 4096;
    }

    // Map 3GB above -> Where the kernel (we are :) ) at 
    i = 0; frame = 0x100000;virt=0xC0000000;
    while (i < 1024)
    {
        pt_entry page = 0;
        pt_entry_set_attrib(&page, I86_PTE_PRESENT);
        pt_entry_set_frame(&page , frame);
        table2->m_entries[i]= page;
        ++i;
        frame += 4096;
        virt += 4096;
    }

    // ! Create Page Directory Table
    pdirectory * dir = (pdirectory *) pmmngr_alloc_block();
    if (!dir){
        return;
    }

    memset(dir, 0, sizeof(pdirectory));

    // Put the 2 table address + set attribute to the Page directory entry 

    pd_entry * entry_0 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
    pd_entry_set_attrib(entry_0, I86_PDE_PRESENT);
    pd_entry_set_attrib(entry_0, I86_PDE_WRITABLE);
    pd_entry_set_frame(entry_0, (physical_addr) table);


    pd_entry * entry_768 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0xC0000000)];
    pd_entry_set_attrib(entry_768, I86_PDE_PRESENT);
    pd_entry_set_attrib(entry_768, I86_PDE_WRITABLE);
    pd_entry_set_frame(entry_768, (physical_addr) table2);

    _cur_pdbr = (physical_addr) &dir->m_entries;

    // Switch to our page directory 
    vmmngr_switch_directory(dir);

    // ! Enable Paging
    pmmngr_paging_enable(true);

} 