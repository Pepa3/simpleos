#ifndef STDDEF_H_INC
#define STDDEF_H_INC

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __WCHAR_TYPE__ wchar_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__ uintptr_t;

#define NULL 0

#define offsetof(st, m) ((size_t)&((st *)0)->m)

#endif