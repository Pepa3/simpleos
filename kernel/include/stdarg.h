#ifndef STDARG_H_INC
#define STDARG_H_INC
#define va_list __builtin_va_list
#define va_start(ap,str) __builtin_va_start(ap,str)
#define va_arg(ap,type) __builtin_va_arg(ap,type)
#define va_end(ap) __builtin_va_end(ap)
#endif