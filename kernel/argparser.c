#include <argparser.h>
#include <stddef.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

void parse_args(args_t* args, char* str){
    args_t* cur = args;
    cur->str = (char*)malloc(10);//TODO: Allocate memory according to input

    int k = 0;
    for (size_t i = 0; i < strlen(str); i++){
        if(str[i]==' '){
            cur->str[k] = '\0';
            cur->next = (struct args_t*)malloc(sizeof(args_t));
            cur=cur->next;
            cur->str = (char*)malloc(10);//TODO: Allocate memory according to input
            k=0;
            continue;
        }
        cur->str[k] = str[i];
        k++;
    }
    cur->str[k] = '\0';
}

void free_args(args_t* args){
    free(args);
    //TODO:free memory occupied by args
}