#ifndef _DEBUGDISPLAY_H
#define _DEBUGDISPLAY_H


extern void     DebugClrScr (const unsigned short c);
extern void     DebugPuts (char* str);
extern int      DebugPrintf (const char* str, ...);
extern unsigned DebugSetColor (const unsigned c);
extern void     DebugGotoXY (unsigned x, unsigned y);
extern void     DebugPutc (unsigned char c);
extern void     DebugGetXY(unsigned int * x, unsigned int * y);
extern int      DebugGetHorizontal();
extern int      DebugGetVertical();


#endif