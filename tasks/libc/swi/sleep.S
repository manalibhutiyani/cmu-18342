/** @file sleep.S
 *
 * @brief sleep sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>

	.file "sleep.S"

FUNC(sleep)
    swi SLEEP_SWI   @ no return value

    @ use an infinite loop to wait the timer expires
    stmfd sp!, {r0-r3}
    @ address of OIER
    ldr r2, =0x40A0001C
loop:
    @ load OIER value
    ldr r3, [r2]
    and r3, r3, #1
    cmp r3, #1
    beq loop
    ldmfd sp!, {r0-r3}
    @ return to user app
    mov pc, lr

