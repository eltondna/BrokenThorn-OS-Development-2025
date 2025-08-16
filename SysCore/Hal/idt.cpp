#include "idt.h"
#include "../Header/string.h"
#include "../Header/Hal.h"

#ifdef _DEBUG
#include "../Kernel/DebugDisplay.h"
#endif

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//! describes the structure for the processors idtr register
struct idtr {

	//! size of the interrupt descriptor table (idt)
	uint16_t		limit;

	//! base address of idt
	uint32_t		base;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif

//! interrupt descriptor table
static struct idt_descriptor	_idt [I86_MAX_INTERRUPTS];

//! idtr structure used to help define the cpu's idtr register
static struct idtr				_idtr;

//! installs idtr into processors idtr register
static void idt_install ();

//! default int handler used to catch unregistered interrupts
static void i86_default_handler ();

//! installs idtr into processors idtr register
static void idt_install () {

	_asm lidt [_idtr]

}

//! default handler to catch unhandled system interrupts.
static void i86_default_handler () {

#ifdef _DEBUG
	DebugGotoXY(0, 10);
	// DebugClrScr (0x18);
	// DebugGotoXY (0,0);
	// DebugSetColor (0x1e);
	DebugPuts ("*** [i86 Hal] i86_default_handler: Unhandled Exception");
#endif
	for(;;);
}


//! returns interrupt descriptor
idt_descriptor* i86_get_ir (uint32_t i) {

	if (i>I86_MAX_INTERRUPTS)
		return 0;

	return &_idt[i];
}


//! installs a new interrupt handler
int i86_install_ir (uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq) {

	if (i>I86_MAX_INTERRUPTS)
		return 0;

	if (!irq)
		return 0;

	//! get base address of interrupt handler
	uint32_t     uiBase = (uint32_t)(irq);

	//! store base address into idt
	_idt[i].baseLo		=	uint16_t(uiBase & 0xffff);
	_idt[i].baseHi		=	uint16_t((uiBase >> 16) & 0xffff);
	_idt[i].reserved	=	0;
	_idt[i].flag		=	uint8_t(flags);
	_idt[i].sel			=	sel;

	return	0;
}


//! initialize idt
int i86_idt_initialize (uint16_t codeSel) {

	//! set up idtr for processor
	_idtr.limit = sizeof (struct idt_descriptor) * I86_MAX_INTERRUPTS -1;
	_idtr.base	= (uint32_t)&_idt[0];

	//! null out the idt
	memset ((void*)&_idt[0], 0, sizeof (idt_descriptor) * I86_MAX_INTERRUPTS-1);

	//! register default handlers
	for (int i=0; i<I86_MAX_INTERRUPTS; i++)
		i86_install_ir (i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)i86_default_handler);

	//! install our idt
	idt_install ();

	return 0;
}
