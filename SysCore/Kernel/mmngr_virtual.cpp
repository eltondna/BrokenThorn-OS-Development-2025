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
    

}

void MmMapPage(void* phys,void* virt){

}
void vmmngr_initialize(){

}
void vmmngr_alloc_page(pt_entry * e){

}
void vmmngr_free_page(pt_entry * e){

}

void vmmngr_get_directory(){

}
void vmmngr_flush_TLB(virtual_addr addr){

}
uint32_t vmmngr_ptable_virt_to_idx(virtual_addr addr){

}
uint32_t vmmngr_pdirectory_virt_to_idx(virtual_addr addr){

}
void vmmngr_ptable_clear(ptable * p){

}
void vmmngr_pdirectory_clear(pdirectory* dir ){

}


