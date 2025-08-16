#include "../Header/size_t.h"
#include "../Header/string.h"

size_t strlen(const char* str){
    size_t len = 0;
    while (str[len] != '\0'){
        len++;
    }
    return len;
}

//! warning C4706: assignment within conditional expression
#pragma warning (disable:4706)

char * strcpy(char* s1, const char* s2){
    char * s1_p = s1;
    while (*s2){
        *s1++ = *s2++;
    }
    *s1 = '\0'; 
    return s1_p;
}


void * memcpy(void * dest, const void * src, size_t count){
    // Cast to unsigned for safety and generic reason
    const unsigned char* sp = (const unsigned char*) src;
    unsigned char* dp = (unsigned char*) dest;
    for (; count > 0; count--){
        *dp++ = *sp++;
    }
    return dest;
}

void * memset(void * dest, char val, size_t count){
    // Cast to unsigned for safety and generic reason
    unsigned char * dp = (unsigned char *) dest;
    while (count-- > 0){
        *dp++ = (unsigned char) val;
    }
    return dest;
}
unsigned short* memsetw(unsigned short * dest, unsigned short val, size_t count){
    unsigned short * dp = (unsigned short* ) dest;
    while (count-- > 0){
        *dp++ = val;
    } 
    return dest;
}
