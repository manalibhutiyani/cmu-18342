@ exit.S: exit sycall wrapper
@
@ Authors  : Hsueh-Hung Cheng (xuehung@gmail.com)
@ Andrew ID: hsuehhuc
@ Date     : 2014-10-02 22:32

#include <bits/swi.h>

	.file	"exit.S"
	.text

	.global	exit
exit:
	swi EXIT_SWI
	mov pc, lr
