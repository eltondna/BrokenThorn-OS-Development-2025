#include "stdint.h"
#include "mmngr_phys.h"
#include "DebugDisplay.h"
#include "string.h"


#define PMMNGR_BLOCKS_PER_BYTE      8
#define PMMNGR_BLOCK_SIZE           4096
#define PMMNGR_BLOCK_ALIGN          PMMNGR_BLOCK_SIZE

static uint32_t   _mmngr_memory_size = 0;
static uint32_t   _mmngr_used_blocks = 0;
static uint32_t   _mmngr_max_blocks  = 0;
static uint32_t*  _mmngr_memory_map  = 0;

// Inline private function
inline void mmap_set(int);
inline void mmap_unset(int);
inline bool mmap_test(int);
int mmap_first_free_block();
int mmap_first_free_region(int size);


// 63
inline void mmap_set(int bit){
    _mmngr_memory_map[bit / 32] |= (1 << (bit % 32));
}

inline void mmap_unset(int bit){
    _mmngr_memory_map[bit/32] &= ~ (1 << (bit % 32));
}

//! test if any bit (frame) is set within the memory map bit array
inline bool mmap_test(int bit){
    return _mmngr_memory_map[bit / 32] &  (1 << (bit % 32));
}

int mmap_first_free_block(){
    int total = pmmngr_get_total_block_count();
    uint32_t bitmap_size = (total + 31u) >> 5;

    for (uint32_t i = 0; i < bitmap_size; i++){
        if (_mmngr_memory_map[i] != 0xffffffff){
            for (int j = 0; j < 32; j++){

                if (i << 5 + j > total)
                    return -1;

                int bit = 1 << j;
                if (!(_mmngr_memory_map[i] & bit))
                    return (i << 5) + j;       // 1 byte = 32 blocks 
            }

        }
    }
    return -1;
}
int mmap_first_free_region(int size){
    if (size == 0)
        return -1;
    if (size == 1)
        return mmap_first_free_block();

    int total = pmmngr_get_total_block_count();
    if (size > total) 
        return -1;
    
    // Round up trick for integer
    uint32_t bitmap_size = (total + 31u) >> 5;
    
    for (uint32_t i =0; i < bitmap_size; i++){
        if (_mmngr_memory_map[i] != 0xffffffff){
            for (int j =0; j < 32; j++){
                int startingbit = (i << 5) + j;
                // No enough memory
                if (startingbit + size > total)
                    return -1;

                int bit = 1 << j;
                if (_mmngr_memory_map[i] & bit)
                    continue;
                
                // Check consecutive free memory block
                uint32_t count = 1;
                for (; count < size; count++){
                    if (mmap_test(startingbit + count))
                        break;
                }
                if (count == size)
                    return startingbit;
            }
        }
    }
    return -1;
}

// MemSize is in KB
void pmmngr_init(size_t memSize, physical_addr bitmap){
    _mmngr_memory_size  = memSize;
    _mmngr_memory_map   = (uint32_t *) bitmap;
    _mmngr_max_blocks   = (pmmngr_get_memory_size() * 1024) / PMMNGR_BLOCK_SIZE;
    _mmngr_used_blocks  = _mmngr_max_blocks;

    DebugPrintf("Init Stage: Total RAM: %dKB, Total 4KB block: %d, Total used blocks: %d\n",memSize,_mmngr_max_blocks, _mmngr_used_blocks);

    // ! By default all memor y is in use
    memset(_mmngr_memory_map, 0xf, pmmngr_get_total_block_count() / PMMNGR_BLOCKS_PER_BYTE);
}

void pmmngr_init_region(size_t size, physical_addr base){
    int align = base / PMMNGR_BLOCK_SIZE;
    int block = size / PMMNGR_BLOCK_SIZE;

    for (; block >= 0; block--){
        mmap_unset(align++);
        --_mmngr_used_blocks;
    }
    // First 4KB is always set
    mmap_set(0);
}
void pmmngr_deinit_region(size_t size, physical_addr base){
    int align = base / PMMNGR_BLOCK_SIZE;
    int block = size / PMMNGR_BLOCK_SIZE;

    for (; block >= 0; block--){
        mmap_unset(align++);
        ++_mmngr_used_blocks;
    }
}

void* pmmngr_alloc_block(){
    if (pmmngr_get_free_block_count() <= 0)
        return 0;
    int frame = mmap_first_free_block();
    if (frame == -1)
        return 0;
    
    mmap_set(frame);
    physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
    ++_mmngr_used_blocks;

    return (void *) addr;

}
void pmmngr_free_block(void* p){
    physical_addr addr = (physical_addr) p;
    int frame = addr / PMMNGR_BLOCK_SIZE;
    mmap_unset(frame);

    --_mmngr_used_blocks;
}
void* pmmngr_alloc_regions(size_t size){
    if (pmmngr_get_free_block_count() <= size)
        return 0;
    int frame = mmap_first_free_region(size);
    if (frame == -1)
        return 0;
    
    for (uint32_t i = 0; i < size; i++){
        mmap_set(frame + i);
    }

    physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
    _mmngr_used_blocks+=size;
    return (void *) addr;
}
void pmmngr_free_regions(void* p, size_t size){
    physical_addr addr = (physical_addr) p;
    int frame = addr / PMMNGR_BLOCK_SIZE;

    for (uint32_t i = 0; i < size; i++){
        mmap_unset(frame + i);
    }
    _mmngr_used_blocks -= size;
}

uint32_t pmmngr_get_use_block_count(){
    return _mmngr_used_blocks;
}

size_t pmmngr_get_memory_size(){
    return _mmngr_memory_size;
}

uint32_t pmmngr_get_free_block_count(){
    return _mmngr_max_blocks - _mmngr_used_blocks;
}

uint32_t pmmngr_get_total_block_count(){
    return _mmngr_max_blocks;
}

uint32_t pmmngr_get_block_size (){
    return PMMNGR_BLOCK_SIZE;
}


void pmmngr_paging_table(bool b){
    #ifdef _MSC_VER
    _asm {
        mov eax, cr0
        cmp [b], 1
        je enable
    disable:
        and eax, 0x7FFFFFFF // Clear bit 31
        mov cr0, eax
        jmp done
    enable:
        or eax, 0x80000000  // Set bit 31
        mov cr0, eax
    done:
    }        
    #endif
}
bool pmmngr_is_paging(){
    uint32_t res = 0;
    #ifdef _MSC_VER
        _asm{
            mov eax, cr0
            mov [res], eax
        }
    #endif
    return (res & 0x80000000) ? true : false;
}

void pmmngr_load_PDBR(physical_addr addr){
    #ifdef _MSC_VER
        _asm{
            mov eax, [addr]
            mov cr3, eax
        }
    #endif
}

physical_addr pmmngr_get_PDBR(){
    #ifdef _MSC_VER
    _asm{
        mov eax, cr3
        ret
    }
    #endif
}