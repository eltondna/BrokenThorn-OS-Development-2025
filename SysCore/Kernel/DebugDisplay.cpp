#include "DebugDisplay.h"
#include "../Header/stdarg.h"
#include "../Header/string.h"


#define VID_MEMORY    0xB8000

static unsigned int _xPos = 0, _yPos = 0;
static unsigned int _startX = 0, _startY = 0;
static unsigned _color = 0;


#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif


void DebugPutc (unsigned char c){
    if (c == 0)
        return;
    // Start a new line
    if (c == '\n' || c == '\r'){
        _yPos += 1;
        _xPos = _startX;
        return;
    }

    // Start a new line
    if (_xPos > 79){
        _yPos += 1;
        _xPos = _startX;
        return;
    }

    /* Print character on screen */
    unsigned char* p = (unsigned char *) VID_MEMORY + (_yPos * 80 + _xPos) * 2;
    *p++ = c;
    *p = _color;
    _xPos++;
}


char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// Unsigned String to int
void itoa(unsigned i, unsigned base, char* buf){
    int pos = 0;
    if (i == 0 || base < 2 || base > 16){
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (i != 0){
        tbuf[pos] = bchars[i % base];
        ++pos;
        i /= base;
    }
    int j;
    int opos = 0;
    for (j = pos-1; j >= 0; j--){
        buf[opos++] = tbuf[j];
    }
    buf[opos] = 0;
}
// Signed String to int
void itoa_s(int i, unsigned base, char* buf){
    if (base > 16) return;
    if (i < 0){
        *buf++ = '-';
        i *= -1;
    }
    itoa(i, base, buf);
}


unsigned DebugSetColor(const unsigned c){
    unsigned t = _color;
    _color = c;
    return t;
}


void  DebugGotoXY (unsigned x, unsigned y){
    _xPos = x * 2;
    _yPos = y * 2;
    _startX = _xPos;
    _startY = _yPos;
}

void DebugClrScr (const unsigned short c) {
    unsigned char* p = (unsigned char* ) VID_MEMORY;
    for (int i = 0; i < 160 * 30; i +=2){
        p[i] = ' ';
        p[i+1] = c;
    }

    // Go to previous set vector
    _xPos = _startX;
    _yPos = _startY;
}


void DebugPuts (char* str){
    if (!str) return;

    /* Debug purpose : Check whether the length of the string */
    // char buf[16];
	// itoa_s(strlen(str), 10, buf);

    for (size_t i = 0; i < strlen(str); i++){
        DebugPutc(str[i]);
    }
}


int DebugPrintf (const char* str, ...){
    if (!str) return 0;

    va_list args;
    va_start(args, str);

    for (size_t i = 0; i < strlen(str); i++){
        switch (str[i]){
            case '%':
                switch(str[i+1]){
                    case 'c': {
                        int v = va_arg(args, int);
                        DebugPutc((char)v);
                        i++;
                        break;
                    }

                    case 's':{
                        char* s = va_arg(args, char*);
                        DebugPuts(s);
                        i++;
                        break;
                    }

                    case 'd':
                    case 'i':{
                        int c = va_arg(args, int);
                        char buf[32] = {0};
                        itoa_s(c, 10, buf);
                        DebugPuts(buf);
                        i++;
                        break;
                    }

                    case 'X':
                    case 'x':{
                        int c = va_arg(args, int );
                        char str[32] = {0};
                        itoa_s(c, 16, str);
                        DebugPuts(str);
                        i++;
                        break;
                    }
 
                    default:
                        va_end(args);
                        return 1;
                }
                break;

            default:
                DebugPutc(str[i]);
                break;
        }
    }
    va_end(args);
    return 0;
}


