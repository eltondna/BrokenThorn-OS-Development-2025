#ifndef _FLPYDSK_DRIVER_H
#define _FLPYDSK_DRIVER_H
#include "../Header/stdint.h"

void        flpydsk_install(int irq);
void        flpydsk_set_working_drive(uint8_t drive);
uint8_t     flpydsk_get_working_drive();
uint8_t*    flpydsk_read_sector(int sectorLBA);
void        flpydsk_lb_to_chs(int lba, int * head, int *track, int *sector);

#endif