#ifndef STRING_H_INC
#define STRING_H_INC
#include <stddef.h>
void itoa(unsigned int i, unsigned int base, char* buf);
void itoa_s(int i, unsigned int base, char* buf);
void reverse(char s[]);
size_t strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
#endif