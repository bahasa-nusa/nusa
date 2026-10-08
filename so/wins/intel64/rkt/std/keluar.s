.section .text

.globl keluar

.extern ExitProcess

keluar:
    pushq %rbp
    movq %rsp, %rbp
    
    call ExitProcess

    leave
    ret
    