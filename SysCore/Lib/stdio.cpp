#include "../Header/stdio.h"
#include "../Header/string.h"
#include "../Header/stdarg.h"
#include "../Header/ctype.h"
#include "../Header/limits.h"


extern void itoa_s(int i,unsigned base,char* buf);

// Check inside main -> may be buggy
int vsprintf(char *str, const char *format, va_list ap){
    if (!str)
        return 0;
    if (!format)
        return 0;

    size_t loc = 0;
    size_t i;

    for (i = 0; i < strlen(format); i++, loc++){
        switch(format[i]){
            case '%':
                switch (format[i+1]){
                    /* Character*/
                    case 'c':{
                        char c = va_arg(ap, char);
                        str[loc] = c;
                        i++;
                        break;
                    }
                       
                    case 'd':
                    case 'i':{
                        int c = va_arg(ap, int);
                        char s[32] = {0};
                        itoa_s(c, 10, s);
                        strcpy(&str[loc], s);
                        loc += strlen(s) - 2; 
                        i++;
                        break;
                    }

                    case 'X':
                    case 'x':{
                        int c = va_arg(ap, int);
                        char s[32] = {0};
                        itoa_s(c, 16, s);
                        strcpy(&str[loc], s);
                        loc += strlen(s) - 2; 
                        i++;
                        break;
                    }

                    case 's':{
                        const char * s = va_arg(ap, const char*);
                        if (!s) s = "(null)";
                        strcpy(&str[loc], s);
                        loc += strlen(s) - 2; 
                        i++;
                        break;
                    }
                }
                break;

            default:
                str[loc] = format[i];
                break;
        }
    }
    return loc;
}

long 
strtol(const char * nptr, char ** endptr, int base){
    const char * s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0;
    int any, cutlim;

    /* 
        * Trim whitespace and pick up leading + / - sign if any.
        * If the base is 0, allow 0x for hex and 0 for octal, else
        * assume decimal; If base already 16, allow 0x
    */

    // 1. Handle leading whitespace  
    do {
        c = *s++;
    }while (isspace(c));

    // 2. Handle leading sign 
    if (c == '-'){
        neg = 1;
        c = *s++;
    }else if (c == '+'){
        c = *s++;
    }

    // Skip the 0x.....
    if ((base == 0 || base == 16) 
    && (c == '0') 
    && (*s == 'x' || *s == 'X')){
            c = s[1];
            s+=2;
            base = 16;
    // Skip the 0B......
    }else if ((base == 0 || base == 2)
        && (c == '0') 
        && (*s == 'b' || *s == 'B')){
            c = s[1];
            s +=2;
            base = 2;
    }

    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
        * Compute the cutoff value between legal numbers and illegal
        * numbers. That is the largest legal value, divided by the 
        * base. An input number that is greater than this value, if
        * followed by a legal input character, is too big. One that is 
        * equal to this value may be valid or not; The limit between 
        * valid and invalid numbers is then based on the last digit.
        * For instancex, if the range for longs is 
        * [-2147483648...2147483647] and the input base is 10,
        * cutoff will be set to 2147483647 and cutlim to either
        * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated 
        * a value > 2147483647, or equal but the next digit is > 7 (or 8),
        * the number will be too big, and we will return a range error.
    */
    

    cutoff = neg ? - (unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long) base;

    // i.e. Parse 1234 
    // 0 * 10 + 1 -> 1 * 10 + 2 -> 12 * 10 + 3 -> 123 * 10 + 4
    for (acc = 0,any = 0;; c = *s++){
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c-= isupper(c) ? 'A' - 10 : 'a' - 10;
        else break;

        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim) )
            any = -1;
        else{
            any = 1;
            acc *=base;
            acc += c;
        }
    }
    if (any < 0)
        acc = neg ? LONG_MIN : LONG_MAX;
    else if (neg)
        acc = -acc;
    
    if (endptr != 0)
        *endptr = (char *) (any ? s-1 : nptr);
    return (acc);
}
unsigned long 
strtoul(const char *nptr , char ** endptr, int base){
    const char * s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;


    do {
        c = *s++;
    }while (isspace(c));

    if (c == '-'){
        neg = 1;
        c = *s++;
    }else if (c == '+'){
        c = *s++;
    }

    if ((base == 0 || base == 16) && 
        (c == '0') && 
        (*s == 'x' || *s == 'X')){
            c = s[1];
            s += 2;
            base = 16;
    }else if ((base == 0 || base == 2) && 
              (c == '0') &&
              (*s == 'b' && *s == 'B')){
            c = s[1];
            s += 2;
            base = 2;
    }


    if (base == 0)
        base = c == '0' ? 8 : 10;
    
    cutoff = (unsigned long) ULONG_MAX / (unsigned long) base;
    cutlim = (unsigned long) ULONG_MAX % (unsigned long) base;

    for (acc = 0, any = 0;; c = *s++){
        if (isdigit(c))
            c -='0';
        else if (isalpha(c))
            c-= isupper(c) ? 'A' - 10 : 'a' - 10;
        else break;

        if (c >= base)
            break;

        // 1. Use the power to determine whether overflow
        // 2. Also the current digit + power 
        if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
            any = -1;
        else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0)
        acc = ULONG_MAX;
    else if (neg)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *) (any ? s - 1 : nptr);
    return (acc);
}
int 
atoi(const char * str){
    return (int) strtol(str,0,10);
}