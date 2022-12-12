[bits 32]
[global _lidt]
[global _printX]
[global __idt_default_handler]

;_lidt:
;    lidt [0x800]
;    ret

_printX:
    mov ebx, 0xb8000
    mov al, 'X'
    mov ah, 0x0f
    mov [ebx], ax
    ret

;__idt_default_handler:
;	pusha
;	mov al, 0x20
;	mov dx, 0x20
;	out dx, al
;	popa
;	iret