/** @file period.S
 *
 * @brief sleep sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>

	.file "period.S"

FUNC(period)
    swi PERIOD_SWI   @ no return value
    mov pc, lr

