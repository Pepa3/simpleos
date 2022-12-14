#ifndef STRING_H_INC
#define STRING_H_INC
void itoa(unsigned i,unsigned base,char* buf);
void itoa_s(int i,unsigned base,char* buf);
void reverse(char s[]);
int strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
#endif