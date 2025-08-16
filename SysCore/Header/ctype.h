#ifndef __CTYPE_H
#define	__CTYPE_H

#ifdef _MSC_VER
// Get rid of conversion warnings
#pragma warning (disable:4244)
#endif


#ifdef __cplusplus
extern "C"{
#endif 

/* CType is an array that store the property of each ascii code*/
/* it is of size 257, since index 0 is reserved for EOF -> -1 (Convention) , */
/* then index 1 is for '\0'*/

extern char _ctype[];

/* Constants*/
#define CT_UP       0x01    /* Uppercase*/
#define CT_LOW      0x02
#define CT_DIG      0x04
#define CT_CTL      0x08
#define CT_PUN      0x10
#define CT_WHT      0x20
#define CT_HEX      0x40
#define CT_SP       0x80    /* 0x20 HardSpace*/  

/* Basic marcos*/


#define isalnum(c) ((_ctype + 1)[(unsigned)(c)] & (CT_UP| CT_LOW | CT_DIG))
#define isalpha(c) ((_ctype + 1)[(unsigned)(c)] & (CT_UP| CT_LOW ))
#define iscntrl(c) ((_ctype + 1)[(unsigned)(c)] & (CT_CTL))
#define isdigit(c) ((_ctype + 1)[(unsigned)(c)] & (CT_DIG))
#define isgraph(c) ((_ctype + 1)[(unsigned)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG))
#define islower(c) ((_ctype + 1)[(unsigned)(c)] & (CT_LOW))
#define isupper(c) ((_ctype + 1)[(unsigned)(c)] & (CT_UP))
#define isprint(c) ((_ctype + 1)[(unsigned)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG | CT_SP))
#define ispunct(c) ((_ctype + 1)[(unsigned)(c)] & (CT_PUN))
#define isspace(c) ((_ctype + 1)[(unsigned)(c)] & (CT_WHT))
#define isxdigit(c)	((_ctype + 1)[(unsigned)(c)] & (CT_DIG | CT_HEX))
#define isascii(c)((unsigned)(c) <= 0x7F)
#define toascii(c)	((unsigned)(c) & 0x7F)
#define tolower(c)	(isupper(c) ? c + 'a' - 'A' : c)
#define toupper(c)	(islower(c) ? c + 'A' - 'a' : c)



#ifdef __cplusplus
}
#endif

#endif
