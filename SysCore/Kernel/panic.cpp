#include "../Header/Hal.h"
#include "DebugDisplay.h"
#include "../Header/stdio.h"
#include "../Header/stdarg.h"

//! looks cool ^^
static char* sickpc = " \
                               _______      \n\
                               |.-----.|    \n\
                               ||x . x||    \n\
                               ||_.-._||    \n\
                               `--)-(--`    \n\
                              __[=== o]___  \n\
                             |:::::::::::|\\ \n\
                             `-=========-`()\n\
                                M. O. S.\n\n";

static char* disc = "An internal error was detected. The system has been halted.\n\
Please restart your computer.\n\n";



void _cdecl kernel_panic(const char* fmt, ...){
    disable();

    DebugClrScr(0x13);
    DebugGotoXY(0,0);
    DebugSetColor(0x17);

    DebugPuts(sickpc);
    DebugPuts(disc);
    va_list args;

    char buf[1024];
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    DebugPrintf("*** STOP: %s\n\n", buf);
}