#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

void printf(char* str,...){
	va_list ap;
	va_start(ap, str);
    char* s = 0;
	for(size_t i = 0; i < strlen(str); i++)
	{
		if(str[i] == '%')
		{
			switch(str[i+1])
			{
				/** string **/
				case 's':
					s = va_arg(ap, char*);
					print(s);
					i++;
					continue;
				/** decimal **/
				case 'd': {
					int c = va_arg(ap, int);
					char str2[32] = {0};
					itoa_s(c, 10, str2);
					print(str2);
					i++;
					continue;
				}
				case 'x': {
					int c = va_arg(ap, int);
					char str2[32] = {0};
					itoa(c, 16, str2);
					print(str2);
					i++;
					continue;
				}
				/** character **/
				case 'c': {
					// char gets promoted to int for va_arg!
					// clean it.
					char c = (char)(va_arg(ap, int) & ~0xFFFFFF00);
					print_char(c,-1,-1,0);
					i++;
					continue;
				}
				default:
					break;
			}
		} else {
			print_char(str[i],-1,-1,0);
		}
	}
	va_end(ap);
}
