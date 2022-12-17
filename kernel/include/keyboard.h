#ifndef KEYBOARD_H_INC
#define KEYBOARD_H_INC
#include <stdint.h>

void init_keyboard();
void clear_keyboard_buffer();
char keyboard_get_key();

#endif