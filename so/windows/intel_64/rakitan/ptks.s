.section .text

.globl cetak

.extern GetStdHandle
.extern WriteConsoleA

cetak:
    pushq %rbp
    movq %rsp, %rbp
    subq $56, %rsp
    andq $-16, %rsp
    movq %rcx, %r12
    xorq %r13, %r13

strlen_loop:
    movb (%rcx, %r13), %al
    testb %al, %al
    jz strlen_done
    incq %r13
    jmp strlen_loop

strlen_done:
    movq $-11, %rcx
    call GetStdHandle
    movq %rax, %rcx
    movq %r12, %rdx
    movq %r13, %r8

    sub $8, %rsp
    leaq (%rsp), %r9
    xorq %r10, %r10
    call WriteConsoleA
    
    add $8, %rsp
    leave
    ret

.globl keluar

.extern ExitProcess

keluar:
    pushq %rbp
    movq %rsp, %rbp
    
    call ExitProcess

    leave
    ret
    