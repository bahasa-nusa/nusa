.section .text

.globl _cetak

.extern _GetStdHandle@4
.extern _WriteConsoleA@20

_cetak:
    pushl %ebp
    movl %esp, %ebp
    subl $20, %esp
    
    movl 8(%ebp), %ecx
    xorl %edx, %edx
    
strlen_loop:
    movb (%ecx, %edx), %al
    testb %al, %al
    jz strlen_done
    incl %edx
    jmp strlen_loop
    
strlen_done:
    pushl $-11
    call _GetStdHandle@4
    movl %eax, %esi
    
    pushl $0
    leal -4(%ebp), %eax
    pushl %eax
    pushl %edx
    pushl %ecx
    pushl %esi
    call _WriteConsoleA@20
    
    movl %ebp, %esp
    popl %ebp
    ret
