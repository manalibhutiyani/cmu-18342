@ read.S: read sycall wrapper
@
@ Authors  : Hsueh-Hung Cheng (xuehung@gmail.com)
@ Andrew ID: hsuehhuc
@ Date     : 2014-11-07 16:51

#include <asm.h>
#include <bits/swi.h>

	.file "sleep.S"

FUNC(sleep)
    swi SLEEP_SWI   @ no return value
    mov pc, lr

