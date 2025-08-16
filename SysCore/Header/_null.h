#ifndef __NULL_H
#define __NULL_H

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#	pragma once
#endif

#ifdef NULL
#undef NULL
#endif



#ifdef __cplusplus
extern "C"{
#endif 
#define NULL 0

#ifdef __cplusplus
}
#else
    #define NULL (void*) 0
#endif

#endif