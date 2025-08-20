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

#### 1. Page Table 

#### 2. Page Directory 

#### 3. Virtual Memory Manager

There is a little mistake in the demo I think is a Typo. In the `vmmngr_pdirectory_lookup_entry` function, the `PAGE_DIRECTORY_INDEX()` macro function should be used instead of the  `PAGE_TABLE_INDEX()`


