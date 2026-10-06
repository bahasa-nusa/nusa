.section .text

.globl _keluar

.extern _ExitProcess@4

_keluar:
    pushl %ebp
    movl %esp, %ebp
    
    pushl $0
    call _ExitProcess@4
    
    popl %ebp
    ret
    