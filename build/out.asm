global _start
_start:
    mov ax, 1
    push ax
    mov ax, 0
    push ax
    add rsp, 2
    mov rax, 2
    push rax
    push QWORD [rsp + 8]
    mov rax, 40
    push rax
    mov rax, 60
    push rax
    pop rax
    pop rbx
    xor rdx, rdx
    div rbx
    push rdx
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    xor rdx, rdx
    div rbx
    push rax
    push QWORD [rsp + 8]
    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall