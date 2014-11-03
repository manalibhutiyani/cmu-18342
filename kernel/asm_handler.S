@ asm_handler.S: Implement handlers including SWI and IRQ
@ 		handlers which will forward control to C code
@
@ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
@ Date:   10/20/2014 14:50

#include <bits/swi.h>

	.file	"asm_handler.S"
	.text

	.global	swi_handler
	.type swi_handler, %function
swi_handler:
	@ leave room on stack for SPSR
	sub	sp, sp, #4

	@ store all non-banked user space registers
	stmfd	sp!, {r0-r12, lr}

	@ backup spsr
	mrs	r2, spsr
	str	r2, [sp, #14*4]

	@ pass the stack pointer as the second arg
	mov	r1, sp

	@ determine the SWI number called
	ldr	r0, [lr,#-4]
	bic	r0, r0, #0xff000000

	@ call C_SWI_handler
	bl	C_SWI_handler

	@ restore spsr
	ldr	r2, [sp, #14*4]
	msr	spsr, r2

	@ restore non-banked user space registers except r0
	@ r0 is the return value from C_SWI_handler
	add	sp, #4
	ldmfd	sp!, {r1-r12, lr}

	@ remove space used to store SPSR
	add	sp, sp, #4

	@ return to user space
	movs	pc, lr


	.global	irq_handler
	.type irq_handler, %function
irq_handler:
    @ initialize IRQ sp
    mov sp, #0xa2000000
	@ leave room on stack for SPSR
	sub	sp, sp, #4

	@ store all non-banked user space registers
	stmfd	sp!, {r0-r12, lr}

	@ backup spsr
	mrs	r2, spsr
	str	r2, [sp, #14*4]

	@ pass the stack pointer as the second arg
	mov	r1, sp

	@ call C_IRQ_handler
	bl	C_IRQ_handler

	@ restore spsr
	ldr	r2, [sp, #14*4]
	msr	spsr, r2

	@ restore non-banked user space registers except r0
	@ r0 is the return value from C_IRQ_handler
	add	sp, #4
	ldmfd	sp!, {r1-r12, lr}

	@ remove space used to store SPSR
	add	sp, sp, #4

	@ return to user space
	movs	pc, lr
