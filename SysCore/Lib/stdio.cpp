#include "../Header/stdio.h"
#include "../Header/string.h"
#include "../Header/stdarg.h"


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