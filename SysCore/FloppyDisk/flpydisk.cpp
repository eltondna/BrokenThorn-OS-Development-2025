#include "../Header/Hal.h"
#include "flpydisk.h"



enum FLPYDSK_IO{
    FLPYDSK_DOR     = 0x3F2,    // Digital Output Register
    FLPYDSK_MSR     = 0x3F4,    // Main Status Register
    FLPYDSK_FIFO    = 0x3F5,    // Data Register
    FLPYDSK_CTRL    = 0x3F7     // Control Register
};


/* FDC Command */

// Bits 0-4 of command byte
enum FLPYDSK_CMD{
    FDC_CMD_READ_TRACK      = 2,     // Read all the sectors on the track
    FDC_CMD_SPECIFY         = 3,     // Config Step Rate, Head Load/Unload time
    FDC_CMD_CHECK_STAT      = 4,     // Read Drive Status
    FDC_CMD_WRITE_SECT      = 5,     // Write to disk 
    FDC_CMD_READ_SECT       = 6,     // Read from disk
    FDC_CMD_CALIBRATE       = 7,     // Move to Cylinder 0
    FDC_CMD_CHECK_INT       = 8,     // Acknowledge interrupt from disk controller
    FDC_CMD_WRITE_DEL_S     = 9,     // Rare used 
    FDC_CMD_READ_ID_S       = 0xA,   // Read Current Sector ID
    FDC_CMD_READ_DEL_S      = 0xC,   // Rare used
    FDC_CMD_FORMAT_TRACK    = 0xD,   // Write Meta data (i.e. GAP fields) and destroy date on track 
    FDC_CMD_SEEK            = 0xF    
};

// Extension command -> Could be used with command byte
enum FLPYDSK_CMD_EXT{
    FDC_CMD_EXT_SKIP        = 0x20,
    FDC_CMD_EXT_DENSITY     = 0x40,  // Computer usually set this
    FDC_CMD_EXT_MULTITRACK  = 0x80
};


/* Digital Output Register */
enum FLPYDSK_DOR_MASK{
    FLPYDSK_DOR_MASK_DRIVE0         = 0,
    FLPYDSK_DOR_MASK_DRIVE1         = 1,
    FLPYDSK_DOR_MASK_DRIVE2         = 2,
    FLPYDSK_DOR_MASK_DRIVE3         = 3,
    FLPYDSK_DOR_MASK_RESET          = 4,
    FLPYDSK_DOR_MASK_DMA            = 8,
    FLPYDSK_DOR_MASK_DRIVE0_MOTOR   = 16,
    FLPYDSK_DOR_MASK_DRIVE1_MOTOR   = 32,
    FLPYDSK_DOR_MASK_DRIVE2_MOTOR   = 64,
    FLPYDSK_DOR_MASK_DRIVE3_MOTOR   = 128
};

/* Main Status Register */

enum FLPYDSK_MSR_MASK{
    FLPYDSK_MSR_MASK_DRIVE1_POS_MODE = 1,
    FLPYDSK_MSR_MASK_DRIVE2_POS_MODE = 2,
    FLPYDSK_MSR_MASK_DRIVE3_POS_MODE = 4,
    FLPYDSK_MSR_MASK_DRIVE4_POS_MODE = 8,
    FLPYDSK_MSR_MASK_BUSY            = 16,
    FLPYDSK_MSR_MASK_DMA             = 32,
    FLPYDSK_MSR_MASK_DATAIO          = 64,
    FLPYDSK_MSR_MASK_DATAREG         = 128
};

/* Controller Status Part 0 */
enum FLPYDSK_ST0_MASK{
    FLPYDSK_ST0_MASK_DRIVE0           = 0,
    FLPYDSK_ST0_MASK_DRIVE1           = 1,
    FLPYDSK_ST0_MASK_DRIVE2           = 2,
    FLPYDSK_ST0_MASK_DRIVE3           = 3,
    FLPYDSK_ST0_MASK_HEADACTIVE       = 4,
    FLPYDSK_ST0_MASK_NOTREADY         = 8,
    FLPYDSK_ST0_MASK_UNITCHECK        = 16,
    FLPYDSK_ST0_MASK_SEEKEND          = 32,
    FLPYDSK_ST0_MASK_INTCODE          = 64
};

/* FLPYDSK_ST0_MASK_INTCODE Type */
enum FLPY_ST0_INTCODE_TYP{
    FLPYDSK_ST0_TYP_NORMAL            = 0,
    FLPYDSK_ST0_TYP_ABNORMAL_ERR      = 1,
    FLPYDSK_ST0_TYP_INVALID_ERR       = 2,
    FLPYDSK_ST0_TYP_NOTREADY          = 3
};


/* GAP 3 Sizes */
enum FLPYDSK_GAP3_LENGTH{
    FLPYDSK_GAP3_LENGTH_STD           = 42,
    FLPYDSK_GAP3_LENGTH_5_14          = 32,
    FLPYDSK_GAP3_LENGTH_3_5           = 27
};


/* Formula : 2 ^ sector * 128 */
enum FLPYDSK_SECTOR_DTL{
    FLPYDSK_SECTOR_DTL_128            = 0,
    FLPYDSK_SECTOR_DTL_256            = 1,
    FLPYDSK_SECTOR_DTL_512            = 2,
    FLPYDSK_SECTOR_DTL_1024           = 4
};

/* Constants */
const int FLOPPY_IRQ                    = 6;
const int FLPY_SECTORS_PER_TRACK        = 18;
//! dma tranfer buffer starts here and ends at 0x1000+64k
const int DMA_BUFFER                    = 0x1000;
static uint8_t _CurrentDrive            = 0;

// Set when IRQ fires
static volatile uint8_t _FloppyDiskIRQ = 0;


extern int sleep(int);

/* DMA covered in next chapter */

//! initialize DMA to use phys addr 84k-128k
void flpydsk_initialize_dma () {

	outportb (0x0a,0x06);	//mask dma channel 2
	outportb (0xd8,0xff);	//reset master flip-flop
	outportb (0x04, 0);     //address=0x1000 
	outportb (0x04, 0x10);
	outportb (0xd8, 0xff);  //reset master flip-flop
	outportb (0x05, 0xff);  //count to 0x23ff (number of bytes in a 3.5" floppy disk track)
	outportb (0x05, 0x23);
	outportb (0x80, 0);     //external page register = 0
	outportb (0x0a, 0x02);  //unmask dma channel 2
}

//! prepare the DMA for read transfer
void flpydsk_dma_read () {

	outportb (0x0a, 0x06); //mask dma channel 2
	outportb (0x0b, 0x56); //single transfer, address increment, autoinit, read, channel 2
	outportb (0x0a, 0x02); //unmask dma channel 2
}

//! prepare the DMA for write transfer
void flpydsk_dma_write () {

	outportb (0x0a, 0x06); //mask dma channel 2
	outportb (0x0b, 0x5a); //single transfer, address increment, autoinit, write, channel 2
	outportb (0x0a, 0x02); //unmask dma channel 2
}

/**
*	Basic Controller I/O Routines
*/

uint8_t flpydsk_read_status(){
    return inportb(FLPYDSK_MSR);
}

void flpydsk_write_dor(uint8_t val){
    outportb(FLPYDSK_DOR, val);
}

void flpydsk_send_command(uint8_t cmd){
    for (int i =0; i < 500; i++){
        if (flpydsk_read_status() & FLPYDSK_MSR_MASK_DATAREG)
            return outportb(FLPYDSK_FIFO, cmd);
    }
}

uint8_t flpydsk_read_data(){
    for (int i =0; i < 500; i++){
        if (flpydsk_read_status() & FLPYDSK_MSR_MASK_DATAREG)
            return inportb(FLPYDSK_FIFO);
    }
}

void flpydsk_write_ccr(uint8_t val){
    outportb(FLPYDSK_CTRL, val);
}

//! wait for irq to fire
inline void flpydsk_wait_irq(){
    while (_FloppyDiskIRQ == 0);
    _FloppyDiskIRQ = 0;
}

/* IRQ Handler */
extern "C" interrupt void i86_flpy_irq(){
    _asm pushad
    _FloppyDiskIRQ = 1;
    interruptdone(FLOPPY_IRQ);
    _asm popad
    _asm iretd
}

/* Controller Command Routine */

// ! Check interrupt Status command
// ! Tells the FDC that we have handled the interrupt.
void flpydsk_check_int(uint32_t * st0, uint32_t* cyl){
    flpydsk_send_command(FDC_CMD_CHECK_INT);
    *st0 = flpydsk_read_data();
    *cyl = flpydsk_read_data();
}

// ! Turn the current floppy drive motor on/off
void flpydsk_control_motor(bool b){
    if (_CurrentDrive > 3)
        return;
    uint32_t motor = 0;

    switch (_CurrentDrive){
        case 0:
            motor = FLPYDSK_DOR_MASK_DRIVE0_MOTOR;
            break;
        case 1:
            motor = FLPYDSK_DOR_MASK_DRIVE1_MOTOR;
            break;
        case 2:
            motor = FLPYDSK_DOR_MASK_DRIVE2_MOTOR;
            break;
        case 3:
            motor = FLPYDSK_DOR_MASK_DRIVE3_MOTOR;
            break;
    }

    if (b)
        flpydsk_write_dor(_CurrentDrive | motor | FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA);
    else
        flpydsk_write_dor(FLPYDSK_DOR_MASK_RESET);

    // In all case; Wait a little bit for the motor to spin up / turn off 
    sleep(20);
}

void flpydsk_drive_data(uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma){
    uint32_t data = 0;
    
    // Configure data command
    flpydsk_send_command(FDC_CMD_SPECIFY);
    data = ((stepr & 0xF) << 4 ) | (unloadt & 0xF);
    flpydsk_send_command(data);
    data = (loadt) << 1 | dma == false ? 0 : 1;
    flpydsk_send_command(data);

}

int flpydsk_calibrate(uint32_t drive){
    uint32_t st0, cyl;

    if (drive == 4)
        return -2;
    
    // Turn on the motor
    flpydsk_control_motor(true);

    for (int i =0; i < 10; i++){
        flpydsk_send_command(FDC_CMD_CALIBRATE);
        flpydsk_send_command(drive);
        flpydsk_wait_irq();
        flpydsk_check_int(&st0, &cyl);

        if (!cyl){
            flpydsk_control_motor(false);
            return 0;
        }

        flpydsk_control_motor(false);
        return -1;
    }
}

void 
flpydsk_disable_controller(){
    flpydsk_write_dor(0);
}

void 
flpydsk_enable_controller(){
    flpydsk_write_dor(FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA); 
}


void 
flpydsk_reset(){
    uint32_t st0, cyl;
    flpydsk_disable_controller();
    flpydsk_enable_controller();
    flpydsk_wait_irq();

    // ! Send CHECK_INT / SENSE INTERRUPT command to all drive
    for (int i =0; i < 4; i++){
        flpydsk_check_int(&st0, &cyl);
    }

    // ! Transfer speed 500kb/s
    flpydsk_write_ccr(0);

    // ! Pass mechanical drive info. Step rate = 3ms , unload time = 240 ms , load time = 16ms
    flpydsk_drive_data(3,16,240,true);

    // ! Calibrate the disk
    flpydsk_calibrate(_CurrentDrive);

}

void 
flpydsk_read_sector_imp(uint8_t head , uint8_t track , uint8_t sector){

    // ! Set the DMA for read transfer
    flpydsk_dma_read();

    // ! Read in a sector 
    flpydsk_send_command(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK | FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);

    flpydsk_send_command(head << 2 | _CurrentDrive);
    flpydsk_send_command(track);
    flpydsk_send_command(head);
    flpydsk_send_command(sector);
    flpydsk_send_command(FLPYDSK_SECTOR_DTL_512);
    flpydsk_send_command(((sector + 1) >= FLPY_SECTORS_PER_TRACK ) ? FLPY_SECTORS_PER_TRACK : sector + 1);
    flpydsk_send_command(FLPYDSK_GAP3_LENGTH_3_5);
    // Dont care, just use 512 bytes
    flpydsk_send_command (0xff);
    
    // Wait for DMA finish 
    flpydsk_wait_irq();

	for (int j=0; j<7; j++)
		flpydsk_read_data ();

    uint32_t st0, cyl;
    // ! Let the FDC know we handled interrupt
    flpydsk_check_int(&st0, &cyl);
}

int 
flpydsk_seek(uint32_t cyl, uint32_t head){
    uint32_t st0, cyl0;

    if (_CurrentDrive >= 4)
        return -1;
    
    for (int i =0; i < 10; i++){
        // ! Send the command 
        flpydsk_send_command(FDC_CMD_SEEK);
        flpydsk_send_command((head) << 2 |_CurrentDrive);
        flpydsk_send_command(cyl);

        // ! Wait for the results phase IRQ
        flpydsk_wait_irq();
        flpydsk_check_int(&st0, &cyl0);

        // ! Found the cylinder ? 
        if (cyl0 == cyl)
            return 0;
    }

    return -1;
}

void 
flpydsk_lba_to_chs(int lba, int * head, int *track, int *sector){
    *head   =  (lba % (FLPY_SECTORS_PER_TRACK * 2)) / (FLPY_SECTORS_PER_TRACK);
    *track  = lba / FLPY_SECTORS_PER_TRACK;
    *sector = lba % FLPY_SECTORS_PER_TRACK + 1 ;
}


void        
flpydsk_install(int irq){
   setvect(irq, i86_flpy_irq);

   flpydsk_initialize_dma();

   flpydsk_reset();
   
   flpydsk_drive_data(13, 1 , 0xf, true);
}


void 
flpydsk_set_working_drive(uint8_t drive){
    if (drive < 4)
        _CurrentDrive = drive;
}

uint8_t 
flpydsk_get_working_drive(){
    return _CurrentDrive;
}

uint8_t*
flpydsk_read_sector(int sectorLBA){
    if (_CurrentDrive >= 4)
        return 0;
    
    // Convert LBA sector to CHS
    int head = 0, track = 0, sector = 1;
    flpydsk_lba_to_chs(sectorLBA, &head, &track, &sector);

    // Turn motor on and seek to track
    flpydsk_control_motor(true);
    if (flpydsk_seek(track, head)!= 0)
        return 0;
    flpydsk_read_sector_imp(head, track, sector);
    flpydsk_control_motor(false);

    return (uint8_t *) DMA_BUFFER;
}


