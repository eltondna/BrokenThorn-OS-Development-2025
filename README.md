# Virtual Memory Manager

### Amendment in SysBoot
#### 1. Add Memory.inc
What it does is create a Page Directory for the kernel. And create 2 Page Table 
`PAGE_TABLE_0` and `PAGE_TABLE_768`

- `PAGE_TABLE_768` map 4MB from the start of kernel address (i.e. 0x10000) to higher address `0xC0000000`. So after paging is enabled, `0xC0000000` is translated to `0x10000`

- `PAGE_TABLE_0` do a identity mapping to map first 4MB physical address `0x00000000` to the same virtual address. The benefits of doing this is to allow the bootloader to access the code and data in the low memory region (i.e. 0x0 to 0x00400000). In this way we dont need to worry about moving the previous developed stuff to the higher half kernel.

#### 2. Change IMAGE_PMODE_BASE to 0xC0000000
- Since paging is enabled, we should treat all the address from now on as `virtual`. When we jump to the kernel execution code, we are jumping to `0xC0000000 + offset from the .exe header`


#### Implementation in the Kernel

**1. Page Table Entry**
The Page Table Entry has the following structure:
```cpp 
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
        // high 20 bit address , low 12 bit attrib bit
        I86_PTE_FRAME           = 0xFFFFF000 
    };
```
Bit 7-11 is self-defined, but at this stage it is not used and set. Fun fact, when CPU extract the physical address, it use a mask to extract instead of just extract the high 20 bit. 

i.e. Page Table Entry = 0x9D003 (PRESENT + WRITABLE)
Then the physical address of the frame is 0x9D003 & 0xFFFFF000 -> 0x9D000

Little Knowledge about the `Page Cache`:
On x86, per-page cache behavior is controlled by PWT/PCD/PAT bits. The modes are:

`Write-Back (WB)` — default for normal RAM. Reads and writes are cached; writes hit the cache and are written to RAM later (on eviction). Fastest general-purpose mode.

`Write-Through (WT)` — Reads are cached, but every write also goes straight to RAM. Helpful for some coherence scenarios, but usually slower than WB.

`Uncacheable (UC/UC-)` — Caching disabled; every access goes to memory. Required for MMIO.

`Write-Combining (WC) (via PAT)` — Writes are buffered and merged into bursts; reads are uncached. Great for framebuffers/video; typically better than WT.

**Rules of thumb**: Use WB for regular kernel pages, UC for device MMIO, and WC (if available) for framebuffers. Avoid mapping the same physical page with different cache modes at different virtual addresses.

**2. Page Directory Entry** 
The page directory entry has more or less the same structure as a page table entry. This is deliberately design since remember Page Directory Entry will be a Page Table if 4MB page is activated. So, the high 20 bits are the page table physical address in 4KB mode or the directly 4MB frame starting address in 4MB mode:

```cpp
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
```

#### 3. Virtual Memory Manager

There is a little mistake in the demo I think is a Typo. In the `vmmngr_pdirectory_lookup_entry` function, the `PAGE_DIRECTORY_INDEX()` macro function should be used instead of the  `PAGE_TABLE_INDEX()`.

Also for initializeing the paging function, for allocating memory for the page directory, the tutorial allocate 3 blocks of 4KB for that. But here in my opinion 1 4KB is actually enough. If this is a mistake, feel free to let me know in the discussion of your thought!!



There is nothing specific to say about the Virtual Memory Maager API. Here I just want to summeraise the assembly code and register that are needed for enabling paging:

```asm
Register:
    cr0: & cr0 with 0x80000000 (bit 31) to enable Paging
    cr3: Store the page directory base address

Instruction:
    invlpg addr : Flush the TLB entries of virtual address (addr)
```
