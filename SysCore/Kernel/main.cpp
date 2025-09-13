#include "DebugDisplay.h"
#include "../Header/Hal.h"
#include "boot_info.h"
#include "mmngr_phys.h"
#include "exception.h"
#include "mmngr_virtual.h"
#include "../Keyboard/kybrd.h"
#include "../Header/string.h"

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

uint32_t kernelSize = 0;

void init(multiboot_info* bootinfo, uint32_t memorymapEntryCount){
	
	hal_initialize();
	// Enable first 20 software interrupt 
	enable();
	setvect(0, (void (__cdecl &)(void))divide_by_zero);
	setvect(1, (void(__cdecl &)(void))single_step_trap);
	setvect(2, (void(__cdecl &)(void))nmi_trap);
	setvect(3, (void(__cdecl &)(void))breakpoint_trap);
	setvect(4, (void(__cdecl &)(void))overflow_trap);  
	setvect(5, (void(__cdecl &)(void))bounds_check_fault);
	setvect(6, (void(__cdecl &)(void))invalid_opcode_fault);
	setvect(7, (void(__cdecl &)(void))no_device_fault);
	setvect(8, (void(__cdecl &)(void))double_fault_abort);
	setvect(10, (void(__cdecl &)(void))invalid_tss_fault);
	setvect(11, (void(__cdecl &)(void))no_segment_fault);
	setvect(12, (void(__cdecl &)(void))stack_fault);
	setvect(13, (void(__cdecl &)(void))general_protection_fault);
	setvect(14, (void(__cdecl &)(void))page_fault);
	setvect(16, (void(__cdecl &)(void))fpu_fault);
	setvect(17, (void(__cdecl &)(void))alignment_check_fault);
	setvect(18, (void(__cdecl &)(void))machine_check_fault);
	setvect(19, (void(__cdecl &)(void))simd_fpu_fault);


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

		// DebugPrintf("Region %i Start: 0x%x%x        Length: 0x%x%x        Type:%s\n", i, 
		// 	region[i].start_Hi, region[i].start_Lo, 
		// 	region[i].sizeHi, region[i].sizeLo, 
		// 	strMemoryTypes[region[i].type-1]);
		++i;

		if (totalMemory < start_address + length)
			totalMemory = (start_address + length);
	}
	totalMemory = totalMemory >> 10;
	// DebugPrintf("Total Memory Size: %d\n", totalMemory);
	pmmngr_init(totalMemory, 0x100000 + kernelSize*512);

	i = 0;
	while ( i < memorymapEntryCount){
		if (region[i].type==1)
			pmmngr_init_region(region[i].sizeLo,region[i].start_Lo);
		i++;
	}
	//! deinit the region the kernel is in as its in use
	pmmngr_deinit_region (0x100000, kernelSize*512);

	// DebugPrintf ("\npmm regions initialized: %i allocation blocks; used or reserved blocks: %i\nfree blocks: %i\n",
	// 	pmmngr_get_total_block_count(),  pmmngr_get_use_block_count (), pmmngr_get_free_block_count ());

	vmmngr_initialize();
	kkybrd_install (33);
}


// Avoid use static int tick here with non-constant initializers
// Or else link error occurs
void sleep (int ms){
	int ticks = ms + get_tick_count();
	while (ticks > get_tick_count());
}


KEYCODE getch(){
	KEYCODE key = KEY_UNKNOWN;
	while (key == KEY_UNKNOWN){
		key = kkybrd_get_last_key();
	}

	kkybrd_discard_last_key();
	return key;
}

void cmd(){
	DebugPrintf("\n$> ");

}


void get_cmd(char * buf, int n){
	cmd();
	KEYCODE key = KEY_UNKNOWN;
	bool bufChar;
	int i = 0;
	
	unsigned int startX = 0, startY = 0;
	DebugGetStartXY(&startX, &startY);
	int startPoint = startX + startY * 80;
	while (i < n){

		bufChar = true;
		key = getch();

		if (key == KEY_RETURN)
			break;
		if (key == KEY_BACKSPACE){
			bufChar = false;

			if (i > 0){
				unsigned int x = 0, y = 0;
				DebugGetXY(&x, &y);
				

				int offset = x + y * 80; 
				if (offset - startPoint > 0){
					if (x > 0){
						--x;
						DebugGotoXY(x, y);
					}
					else {
						--y;
						x = DebugGetHorizontal();
						DebugGotoXY(x, y);
					}
					DebugPutc(' ', false);
				}

				i--;
			}


		}
		if (bufChar){
			char c = kkybrd_key_to_ascii(key);
			if (c != 0){
				DebugPutc(c, true);
				buf[i++] = c;
			}
		}
	}
	buf[i] = '\0';
}
bool run_cmd(char* cmd_buf){
	if (strcmp(cmd_buf, "exit") == 0)
		return true;
	
	if (strcmp(cmd_buf, "cls") == 0)
		DebugClrScr(0x17);
	else if (strcmp(cmd_buf, "help") == 0){
		DebugPuts ("\nOS Development Series Keyboard Programming Demo");
		DebugPuts ("\nwith a basic Command Line Interface (CLI)\n\n");
		DebugPuts ("Supported commands:\n");
		DebugPuts (" - exit: quits and halts the system\n");
		DebugPuts (" - cls: clears the display\n");
		DebugPuts (" - help: displays this message\n");
	}else DebugPrintf("\nUnknown Command");

	return false;
}


void run(){
	char cmd_buf[256];

	while (1){
		get_cmd(cmd_buf, 254);

		if (run_cmd(cmd_buf) == true)
			break;
	}
}

int _cdecl main (multiboot_info* bootinfo, uint32_t memorymapEntryCount) {

	_asm mov word ptr [kernelSize], dx
	init(bootinfo, memorymapEntryCount);
	
	DebugClrScr(0x17);
	DebugGotoXY(0,0);
	DebugPuts("Type \"exit\" to quit \"help\" for a list of commands\n");
	DebugPuts("+----------------------------------------------------+\n");

	run();
	DebugPrintf("\n Exit Command received; System Halted");
	for (;;);
	return 0;
}
