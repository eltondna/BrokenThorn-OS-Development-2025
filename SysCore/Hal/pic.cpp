#include "Hal.h"
#include "pic.h"

                            /* 8259 PIC Overview */ 
/*  
Each PIC has 5 functional register :

    *** 3 internal registers *** 
        1. IRR (Interrupt Request Register) : Show pending interrupts
        2. ISR (In-Service Register)        : Show current interrupt being serviced
        3. IMR (Interrupt Mask Register)    : Determines which IRQ lines are disabled

    *** 2 IO register per PIC *** 
        1. Command Register
        2. Data Register

Initialization : 4 Command available

    - ICW 1 : 8259 Configuration
    - ICW 2 : Assign interrupt number to each IRQ 
              -> i.e. ICW2 = 0x20 and send to master PIC: IRQ0 starts from 0x20
                      ICW2 = 0x28 and send to slave PIC : IRQ8 starts from 0x28
    - ICW 3 : Tell PIC how master-slave wired
              -> i.e. ICW3 = 0x04(00000100) and send to master PIC
                      -> slave connected to IRQ2
              -> i.e. ICW3 = 0x02(ID of IRQ line connected to on the master) and send to slave
                      -> slave raise an IRQ via IRQ2 on master PIC
    - ICW4  : 8259 configuration


*/ 

#define I86_PIC1_REG_COMMAND    0x20
#define I86_PIC1_REG_STATUS     0x20
#define I86_PIC1_REG_DATA       0x21
#define I86_PIC1_REG_IMR        0x21


#define I86_PIC2_REG_COMMAND    0xA0
#define I86_PIC2_REG_STATUS     0xA0
#define I86_PIC2_REG_DATA       0xA1
#define I86_PIC2_REG_IMR        0xA1



//-----------------------------------------------
//	Initialization Word Bit Masks
//-----------------------------------------------

#define I86_PIC_ICW1_MASK_IC4   0x1     // 1: IC4 is sent
#define I86_PIC_ICW1_MASK_SNGL  0x2     // 1: 1 PIC 0: 2 PIC
#define I86_PIC_ICW1_MASK_ADL   0x4     // Ignored in X86
#define I86_PIC_ICW1_MASK_LTIM  0x8     // 1: Level 0: edge Triggered
#define I86_PIC_ICW1_MASK_INIT  0x10    // 1: INIT PIC

#define I86_PIC_ICW4_MASK_UPM   0x01    // 1 : 8086 mode
#define I86_PIC_ICW4_MASK_AEOI  0x02    // 1 : Automatically send EOI to CPU
#define I86_PIC_ICW4_MASK_MS    0x04    // 1 : Buffer master 0: Buffer slave (Use when buffer mode is set)
#define I86_PIC_ICW4_MASK_BUF   0x08    // 1 : Buffer Mode
#define I86_PIC_ICW4_MASK_SFNM  0x10    // Special Fully Nested Mode (for the slave behavior)

//-----------------------------------------------
//	Initialization Word Bit Command
//-----------------------------------------------

#define I86_PIC_ICW1_IC4_EXPECT				1			//1
#define I86_PIC_ICW1_IC4_NO					0			//0
#define I86_PIC_ICW1_SNGL_YES				2			//10
#define I86_PIC_ICW1_SNGL_NO				0			//00
#define I86_PIC_ICW1_ADI_CALLINTERVAL4		4			//100
#define I86_PIC_ICW1_ADI_CALLINTERVAL8		0			//000
#define I86_PIC_ICW1_LTIM_LEVELTRIGGERED	8			//1000
#define I86_PIC_ICW1_LTIM_EDGETRIGGERED		0			//0000
#define I86_PIC_ICW1_INIT_YES				0x10		//10000
#define I86_PIC_ICW1_INIT_NO				0			//00000


#define I86_PIC_ICW4_UPM_86MODE			1			
#define I86_PIC_ICW4_UPM_MCSMODE		0			
#define I86_PIC_ICW4_AEOI_AUTOEOI		2			
#define I86_PIC_ICW4_AEOI_NOAUTOEOI		0			
#define I86_PIC_ICW4_MS_BUFFERMASTER	4			
#define I86_PIC_ICW4_MS_BUFFERSLAVE		0			
#define I86_PIC_ICW4_BUF_MODEYES		8			
#define I86_PIC_ICW4_BUF_MODENO			0			
#define I86_PIC_ICW4_SFNM_NESTEDMODE	0x10		
#define I86_PIC_ICW4_SFNM_NOTNESTED		0			


uint8_t i86_pic_read_data(uint8_t picNum){
    if (picNum > 1)
        return 0;
    uint8_t reg = (picNum == 1) ? I86_PIC2_REG_DATA : I86_PIC1_REG_DATA;
    return inportb(reg);
}
void i86_pic_send_data(uint8_t data, uint8_t picNum){
    if (picNum > 1)
        return;
    uint8_t reg = (picNum == 1) ? I86_PIC2_REG_DATA : I86_PIC1_REG_DATA;
    outportb(reg,data);
}
void i86_pic_send_command (uint8_t cmd, uint8_t picNum){
    if (picNum > 1)
        return;
    uint8_t reg = (picNum == 1) ? I86_PIC2_REG_COMMAND : I86_PIC1_REG_COMMAND;
    outportb(reg, cmd);
}


void i86_pic_initialize (uint8_t base0, uint8_t base1){
    uint8_t	icw	= 0;
    
    disable();
    // ICW1
    icw |=I86_PIC_ICW1_INIT_YES;
    icw |= I86_PIC_ICW1_IC4_EXPECT;

    i86_pic_send_command(icw, 0);
    i86_pic_send_command(icw, 1);

    // ICW2 
    i86_pic_send_data(base0, 0);
    i86_pic_send_data(base1, 1);

    // ICW3
    i86_pic_send_data(0x04,0);
    i86_pic_send_data(0x02,1);

    // ICW4
    icw = 0; 
    icw |= I86_PIC_ICW4_UPM_86MODE;

    i86_pic_send_data(icw, 0);
    i86_pic_send_data(icw, 1);

    i86_pic_send_data(0x00, 0);  // Unmask all on master PIC
    i86_pic_send_data(0x00, 1);  // Unmask all on slave PIC

}
char i86_get_pic1_imr(){
	return inportb(I86_PIC1_REG_IMR);
}
