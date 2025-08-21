#include "vmmngr_pte.h"

inline void pt_entry_set_attrib(pt_entry * e, uint32_t attrib){
    *e |= attrib;
}
inline void pt_entry_delete_attrib(pt_entry * e, uint32_t attrib){
    *e &= ~attrib;
}
inline void pt_entry_set_frame(pt_entry * e, physical_addr addr){
    *e = (*e & ~I86_PTE_FRAME) | addr;
}
inline bool pt_entry_is_present(pt_entry e){
    return e & I86_PTE_PRESENT;
}
inline bool pt_entry_is_writable(pt_entry e){
    return e & I86_PTE_WRITABLE;
}
inline physical_addr pt_entry_pfn(pt_entry e){
    return e & I86_PTE_FRAME;
}