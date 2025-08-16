#ifndef __STDARG_H
#define	__STDARG_H

#include "va_list.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Width of stack ==  width of int */
#define STACKITEM int

/* 1. Round up width of objects to multipe of STACKITEM : 4*/
/* 2. For already aligned item, (sizeof(TYPE) + sizeof(STACKITEM) + -1) ensure */
/* that it wont round up */
#define VA_SIZE(TYPE) \
((sizeof(TYPE) + sizeof(STACKITEM) + -1) \
  & ~(sizeof(STACKITEM) - 1) \
) 

/* 1. Move va_list pointing to the first va_arg*/
#define va_start(AP, LASTARG) \
    (AP = ((va_list) &(LASTARG) + VA_SIZE(LASTARG)))

/* Do nothing for va_end */
#define va_end(AP)

/* 1. Move forward the va_list*/
/* 2. Return the value of the va_arg : Expression behind the comma*/
#define va_arg(AP, TYPE) \
    (AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))
    
#ifdef __cplusplus
}
#endif

#endif
