@ crt0.S: Execution startup routine
@
@ Authors  : Hsueh-Hung Cheng (xuehung@gmail.com)
@ Andrew ID: hsuehhuc
@ Date     : 2014-10-02 22:32

#include <bits/swi.h>

	.file	"crt0.S"
	.text

	.global	_start
_start:
	ldr r0, [sp]
	add r1, sp, #4
	bl main
	swi EXIT_SWI
