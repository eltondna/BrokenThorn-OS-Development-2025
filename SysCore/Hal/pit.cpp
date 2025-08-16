#include "../Header/Hal.h"
#include "idt.h"
#include "pit.h"
#include "pic.h"
#include "../Kernel/DebugDisplay.h"


#define I86_PIT_REG_COUNTER0    0x40
#define I86_PIT_REG_COUNTER1    0x41
#define I86_PIT_REG_COUNTER2    0x42
#define I86_PIT_REG_COMMAND     0x43

static volatile uint32_t _pit_ticks     = 0;
static bool              _pit_bIsInit   = false;

static void __cdecl pit_handler_c(void)
{
    ++_pit_ticks;
    interruptdone(0);     
}


extern "C" interrupt void i86_pit_irq(void)
{
    __asm {
        pushad
        call    pit_handler_c        
        popad
        iretd                       
    }
}

void  i86_pit_send_command(uint8_t cmd){
    outportb(I86_PIT_REG_COMMAND, cmd);
}
void  i86_pit_send_data(uint16_t data, uint8_t counter){
    uint8_t port ;
      switch (counter){
        case I86_PIT_OCW_COUNTER_0:{
            port = I86_PIT_REG_COUNTER0;
            break;
        }

        case I86_PIT_OCW_COUNTER_1:{
            port = I86_PIT_REG_COUNTER1;
            break;
        }

        case I86_PIT_OCW_COUNTER_2:{
            port = I86_PIT_REG_COUNTER2;
            break;
        }
    }
    outportb(port, (uint8_t) data);
}

uint8_t i86_pit_read_data(uint16_t counter){
    uint8_t port ;
    switch (counter){
        case I86_PIT_OCW_COUNTER_0:{
            port = I86_PIT_REG_COUNTER0;
            break;
        }

        case I86_PIT_OCW_COUNTER_1:{
            port = I86_PIT_REG_COUNTER1;
            break;
        }

        case I86_PIT_OCW_COUNTER_2:{
            port = I86_PIT_REG_COUNTER2;
            break;
        }
    }
    uint8_t data = inportb(port);
    return data;
}
// Return previous tickCount
uint32_t i86_pit_set_tickcount(uint32_t i){
    uint32_t prev= _pit_ticks;
    _pit_ticks = i;
    return _pit_ticks;
}
uint32_t i86_pit_get_tickcount(){
    return _pit_ticks;
}

void i86_pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode){
    if (freq == 0)
        return;
    
    uint16_t divisor = uint16_t(1193181 / (uint16_t) freq);
    uint8_t ocw = 0;
    ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
    ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
    ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
    i86_pit_send_command(ocw);

    // Low Byte then high byte
    i86_pit_send_data(divisor & 0xFF, counter);
    i86_pit_send_data((divisor >> 8)  & 0xFF, counter);
	_pit_ticks=0;
}


void _cdecl i86_pit_initialize(){
    setvect(32, i86_pit_irq);
    _pit_bIsInit = true;
}

bool _cdecl i86_pit_is_initialized(){
    return _pit_bIsInit;
}