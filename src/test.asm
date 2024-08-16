global _start
_start:
    mov rax, 60
    mov rdi, 2
    syscall
;for compile: 
; nasm -f elf64 -o test.o test.asm && ld -o test test.o