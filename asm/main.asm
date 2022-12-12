[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; This is the memory offset to which we will load our kernel
mov [BOOT_DRIVE], dl ; BIOS stores our boot drive in DL
mov bp, 0x9000 ; Set - up the stack.
mov sp, bp
mov bx, MSG_REAL_MODE
call print_string
call load_kernel
call switch_to_pm ; Switch to protected mode , from which we will not return

jmp $

%include "asm/print_string.asm"
%include "asm/disk_load.asm"
%include "asm/gdt.asm"
%include "asm/print_string_pm.asm"
%include "asm/switch_to_pm.asm"

[bits 16]

load_kernel:
mov bx, MSG_LOAD_KERNEL
call print_string
mov bx, KERNEL_OFFSET   ; Set - up parameters for our disk_load routine , so
mov dh, 20              ; that we load the first 15 sectors (excluding // 20 - 0x2800
mov dl, [BOOT_DRIVE]    ; the boot sector) from the boot disk (i.e. our
call disk_load          ; kernel code) to address KERNEL_OFFSET
ret

[bits 32]
; This is where we arrive after switching to and initialising protected mode.
BEGIN_PM:
mov ebx, MSG_PROT_MODE
call print_string_pm
call KERNEL_OFFSET

jmp $ ; Hang.

BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16 - bit Real Mode" , 0
MSG_PROT_MODE db "Successfully landed in 32 - bit Protected Mode" , 0
MSG_LOAD_KERNEL db "Loading kernel into memory." , 0

; Bootsector padding
times 510-($-$$) db 0
dw 0xaa55