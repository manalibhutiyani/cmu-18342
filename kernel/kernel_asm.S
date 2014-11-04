@ kernel_asm.S: kernel functions implemented in assembly
@
@ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
@ Date:   10/20/2014 14:50

#include <bits/swi.h>
#include <arm/exception.h>
#include <arm/psr.h>

	.file	"kernel_asm.S"
	.text


	.global	switch_user_app
	.type switch_user_app, %function
@ change to user mode and start from specified address
switch_user_app:
	@ r0 = the address standalone application starts
	@ r1 = argc
	@ r2 = argv

	@ store the context
	stmfd	sp!, {r0-r12, lr}
	@ lr will be banked after changing mode, so put in r4
	mov	r4, lr
	@ r5 = the value which sp should be when returing to kernel
	add	r5, sp, #4*14

	@ switch to user mode with IRQs & FIQs masked
	mrs	r3, cpsr
	@orr	r3, r3, #0xC0
	@bic	r3, r3, #0x1F
	@orr	r3, r3, #0x10
    bic r3, r3, #PSR_IRQ
	bic	r3, r3, #PSR_MODE
	orr	r3, r3, #PSR_MODE_USR
	msr	cpsr, r3

	@ set the stack 0xa3000000
	ldr	sp, =0xa3000000

	@ push backup sp, lr_svc (return to main)
	push	{r4, r5}

	@ push argc and argv and null at argv[argc]
	mov r4, #0
	push {r4}
	sub	r4, r1, #1
loop:
	ldr	r5, [r2, r4, lsl #2]
	push 	{r5}
	subs	r4, #1
	bpl	loop
	push	{r1}


	@ jump to r0, should be 0xa2000000 in this case
	mov	pc, r0


	.global back_to_kernel
	.type back_to_kernel, %function
@ return to kernel when exit is called
back_to_kernel:
	@ r0 = exit value

	@ restore the original handler
	stmfd	sp!, {r0-r12, lr}
    mov     r0, #EX_SWI
	bl	restore_handler
    mov     r0, #EX_IRQ
	bl	restore_handler
	ldmfd	sp!, {r0-r12, lr}

	@ get the address to main
	ldr	r1, =0xa3000000
	@ restore sp and lr
	ldr	sp, [r1, #-4]
	ldr	lr, [r1, #-8]

	@ return to main with r0 = exit value
	bx	lr
