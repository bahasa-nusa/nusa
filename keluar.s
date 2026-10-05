.section .text
.globl keluar
keluar:
    pushq %rbp
    movq %rsp, %rbp
    
    call ExitProcess

    leave
    ret
