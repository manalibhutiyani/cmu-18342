@ write.S: write sycall wrapper
@
@ Authors  : Hsueh-Hung Cheng (xuehung@gmail.com)
@ Andrew ID: hsuehhuc
@ Date     : 2014-10-02 22:32

#include <bits/swi.h>
	.file	"write.S"
	.text

	.global	write
write:
        swi WRITE_SWI
        cmp r0, #0      @ check if error occurs
        bpl return
        neg r0, r0      @ get negative of return value
        ldr r3, =errno  @ load the address of variable errno
        str r0, [r3]    @ set appropriate errno value
        mov r0, #-1
return:
        mov pc, lr
