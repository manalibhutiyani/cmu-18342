/*
 * kernel_helpers.c: other kernel functions
 *
 * Author: Hsueh-Hung Cheng <xuehung@gmail.com>
 * Date:   10/20/2014 14:38
 */

#include <exports.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <bits/swi.h>
#include <bits/errno.h>
#include "kernel_helpers.h"

extern void back_to_kernel(int exit_value);

/* save the addresses of handlers */
unsigned *uboot_handler_addr[NUM_EXCEPTIONS - 1];
/* only backup the first two instructions */
unsigned instruction_backup[NUM_EXCEPTIONS - 1][2];

/*
 * This is the handler for IRQ interrupt
 * It first find which devise triggered the interrupt and
 * do corresponding task
 */
int C_IRQ_handler(unsigned swi_number, unsigned *regs) {
    printf("C_IRQ_handler: entering\n");
    return 0;
}

/*
 * This dispatch the appropriate syscalls.
 * Return the return value of the syscall.
 */
int C_SWI_handler(unsigned swi_number, unsigned *regs) {
    printf("entering C_SWI_handler with swi # = %d\n", swi_number);
    int return_val = 0;
    switch (swi_number) {
        case EXIT_SWI:
            back_to_kernel(regs[0]);
            break;
        case READ_SWI:
            return_val = c_read(regs[0], (char *)regs[1], regs[2]);
            break;
        case WRITE_SWI:
            return_val = c_write(regs[0], (char *)regs[1], regs[2]);
            break;
        case TIME_SWI:
            return_val = (int)get_OS_time();
            break;
        case SLEEP_SWI:
            set_sleep((unsigned)regs[0]);
            break;
    }
    return return_val;
}

/*
 * Read from stdin
 */
ssize_t c_read(int fd, void *buf, size_t n) {
    char c;
    char *ptr = (char *)buf;

    if (fd != 0) {
        return -EBADF;
    }
    if (ptr < (char *)SDRAM_START || (ptr + n -1) > (char *)SDRAM_END) {
        return -EFAULT;
    }

    ssize_t offset = 0;
    while ((offset = (ssize_t)(ptr - (char *)buf)) < (ssize_t)n) {
        switch (c = getc()) {
            case 4:	// EOT
                return offset;
            case 8:	// backspace
            case 127: // delete
                if ((void *)ptr > buf) {
                    ptr--;
                    puts("\b \b");
                }
                break;
            case '\n':
            case '\r':
                *(ptr++) = '\n';
                putc('\n');
                return offset;
            default:
                putc(c);
                *(ptr++) = c;
                break;
        }
    }
    return offset;
}

/*
 * write to stdout
 */
ssize_t c_write(int fd, const void *buf, size_t n) {
    int i;
    char *ptr = (char *)buf;

    if (fd != 1) {
        return -EBADF;
    }

    for (i=(signed)n ; i>0 ; i--) {
        putc(*ptr);
        ptr++;
    }
    return (ssize_t)(ptr - (char *)buf);
}

/*
 * wrie in our own handler given the vecotr
 */
int wiring_handler(unsigned vec_num, void *handler_addr) {
    printf("entering wiring_handler with vec_num = %x\n", vec_num);
    unsigned *vector_addr = (unsigned *)(vec_num * 0x4);
    /*
     * check SWI vector contains the valid format:
     * that is ldr pc, [pc, positive #imm12]
     */
    unsigned offset = (*vector_addr) ^ 0xe59ff000;
    if (offset > 0xfff) {
        return -1;
    }
    /* pc is increased by 0x8 when being accessed */
    int *ptr_to_handler_addr = (void *)(offset + (unsigned)vector_addr + 0x08);
    uboot_handler_addr[vec_num] = (void *)(*ptr_to_handler_addr);

    /* backup the original instructions at SWI_handler */
    instruction_backup[vec_num][0] = *(uboot_handler_addr[vec_num]);
    instruction_backup[vec_num][1] = *(uboot_handler_addr[vec_num] + 1);

    /* put new instructions at SWI_handler */
    *(uboot_handler_addr[vec_num]) = 0xe51ff004;       // pc, [pc, #-4]
    *(uboot_handler_addr[vec_num] + 1) = (unsigned int)handler_addr;

    printf("exiting wiring handler\n");
    return 1;
}

/*
 * restore to the original handlers given vector number
 */
void restore_handler(unsigned vec_num) {

    if (vec_num < NUM_EXCEPTIONS) {
        *(uboot_handler_addr[vec_num]) = instruction_backup[vec_num][0];
        *(uboot_handler_addr[vec_num] + 1) = instruction_backup[vec_num][1];
    }
}


unsigned get_OS_time() {
    printf("entering get_OS_time\n");
    unsigned *OS_time = (unsigned *)OSTMR_ADDR(OSTMR_OSCR_ADDR);
    printf("exiting get_OS_time, time = %x\n", *OS_time);
    return *OS_time;
}

void set_sleep(unsigned time) {
    /* covert unit from ms to hz */
    unsigned time_in_hz =  time * 3686.4;
    unsigned final_time = *((unsigned *)OSTMR_ADDR(OSTMR_OSCR_ADDR)) + time_in_hz;
    /* write the value into OSMR0 */
    *((unsigned *)OSTMR_ADDR(OSTMR_OSMR_ADDR(0))) = final_time;
    /* set the interrupt enable register OIER to enable channel 0*/
    *((unsigned *)OSTMR_ADDR(OSTMR_OIER_ADDR)) |= OSTMR_OIER_E0;
    printf("set OSMR %x to %x\n", OSTMR_ADDR(OSTMR_OSMR_ADDR(0)), final_time);
    /* ICLR */
    *((unsigned *)0x40D00008) &= 0x0;
    /* ICMR */
    *((unsigned *)0x40D00004) |= 0x04000000;

}
