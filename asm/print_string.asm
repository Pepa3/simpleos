[bits 16]
print_string:
    mov al, [bx]
    cmp al, 0
    je print_string_end
    mov ah, 0x0e
    int 0x10
    add bx, 1
    jmp print_string
print_string_end:
    ret
