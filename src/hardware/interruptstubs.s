
.set IRQ_BASE, 0x20

.section .text

.extern _ZN4byos8hardware16InterruptManager15HandleInterruptEhj

.macro HandleException num
.global _ZN4byos8hardware16InterruptManager16HandleException\num\()Ev
_ZN4byos8hardware16InterruptManager16HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4byos8hardware16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4byos8hardware16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00  #timer int  
HandleInterruptRequest 0x01  #keyboard int 
HandleInterruptRequest 0x0C  #mouse int 

int_bottom:

    pusha
    pushl %ds 
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call _ZN4byos8hardware16InterruptManager15HandleInterruptEhj
    add %esp, 6
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

.global _ZN4byos8hardware16InterruptManager15IgnoreInterruptEv
_ZN4byos8hardware16InterruptManager15IgnoreInterruptEv:
    iret

.data 
    interruptnumber: .byte 0



