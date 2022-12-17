#ifndef ARGPARSER_H_INC
#define ARGPARSER_H_INC
typedef struct args_t{
    char* str;
    struct args_t* next;
}args_t;

void parse_args(args_t* args, char* str);
void free_args(args_t* args);

#endif