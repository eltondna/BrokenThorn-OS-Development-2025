#include "DebugDisplay.h"
#include "../Header/Hal.h"
#include "boot_info.h"
#include "mmngr_phys.h"


// Format of each memory map entry
struct memory_region{
	uint32_t start_Lo;
	uint32_t start_Hi;
	uint32_t sizeLo;
	uint32_t sizeHi;
	uint32_t type;
	uint32_t acpi_3_0;
};

char * strMemoryTypes [] = {
	{"Available"},
	{"Reserved"},
	{"ACPI Reclaim"},
	{"ACPI NVS Memory"}
};

int _cdecl main (multiboot_info* bootinfo, uint32_t memorymapEntryCount) {
	/* Display Set Up*/
	DebugClrScr (0x18);
	DebugGotoXY (0,0);
	DebugSetColor (0x70);
	DebugPrintf (" Microcomputer Operating System (MOS) Preparing to load...                       ");
	DebugGotoXY (0,1);
	DebugPrintf (" MOS Starting Up...");
	DebugGotoXY (0,2);
	DebugPrintf (" Initializing Hardware Abstraction Layer (HAL.lib)...                           \n");
	hal_initialize();

	
	uint32_t kernelSize = 0;
	_asm mov word ptr [kernelSize], dx

	DebugSetColor (0x19);
	DebugPrintf ("Physical Memory Map: \n");

	// Rmb the memory map info is read to 0x1000?
	memory_region* region = (memory_region *) 0x1000;
	int i =0; 

	DebugSetColor (0x17);
	uint32_t totalMemory = 0;

	while (i < memorymapEntryCount){
		//! sanity check; if type is > 4 mark it reserved
		if (region[i].type > 4)
			region[i].type = 2;

		uint32_t start_address = region[i].start_Lo;
		uint32_t length = region[i].sizeLo;

		DebugPrintf("Region %i Start: 0x%x%x        Length: 0x%x%x        Type:%s\n", i, 
			region[i].start_Hi, region[i].start_Lo, 
			region[i].sizeHi, region[i].sizeLo, 
			strMemoryTypes[region[i].type-1]);
		++i;

		if (totalMemory < start_address + length)
			totalMemory = (start_address + length);
	}
	totalMemory = totalMemory >> 10;
	DebugPrintf("Total Memory Size: %d\n", totalMemory);
	pmmngr_init(totalMemory, 0x100000 + kernelSize*512);

	i = 0;
	while ( i < memorymapEntryCount){
		if (region[i].type==1)
			pmmngr_init_region(region[i].sizeLo,region[i].start_Lo);
		i++;
	}
	//! deinit the region the kernel is in as its in use
	pmmngr_deinit_region (0x100000, kernelSize*512);

	DebugPrintf ("\npmm regions initialized: %i allocation blocks; used or reserved blocks: %i\nfree blocks: %i\n",
		pmmngr_get_total_block_count(),  pmmngr_get_use_block_count (), pmmngr_get_free_block_count ());

	return 0;

}
