@ read.S: period sycall wrapper
@
@ Authors  : Hsueh-Hung Cheng (xuehung@gmail.com)
@ Andrew ID: hsuehhuc
@ Date     : 2014-11-07 16:51

#include <asm.h>
#include <bits/swi.h>

	.file "period.S"

FUNC(period)
    swi PERIOD_SWI   @ no return value
    mov pc, lr

