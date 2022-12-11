#include"screen.h"
void main(){
    clear_screen();
    
    print("Hello,World!\n");
    for(int i = 1; i < 24; i++){
        print_hex(0xeecabeff);
        print("\n");   
    }
    
}