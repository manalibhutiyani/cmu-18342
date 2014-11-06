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
#include <arm/reg.h>
#include <bits/swi.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include "kernel_helpers.h"

extern void back_to_kernel(int exit_value);
extern volatile uint32_t sys_time;

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
    //printf("*****    C_IRQ_handler: entering\n");

    /* find out if the timer cause this interrupt */
    if (reg_read(INT_ICPR_ADDR) & (1 << INT_OSTMR_0)) {
        //printf("ICMR_0 caused this interupt\n");
        sys_time++;
        /* write 1 to this bit to acknowledge the match and clear it */
        reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
        update_timer(TIME_RESOLUTION);
    }

//    printf("C_IRQ_handler: exiting\n");
    return 0;
}

/*
 * This dispatch the appropriate syscalls.
 * Return the return value of the syscall.
 */
int C_SWI_handler(unsigned swi_number, unsigned *regs) {
//    printf("entering C_SWI_handler with swi # = %d\n", swi_number);
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
            return_val = sys_time * TIME_RESOLUTION;
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
ssize_t c_read(int fd, void *buf, size_t count) {
    char c;
    char *ptr = (char *)buf;
    size_t offset = 0;

    if (fd != STDIN_FILENO) {
        return -EBADF;
    }
    if ((unsigned)buf < SDRAM_START || ((unsigned)buf + count -1) > SDRAM_END) {
        return -EFAULT;
    }

    
    while (offset != count) {
        switch (c = getc()) {
            case 4:	// EOT
                return offset;
            case 8:	// backspace
            case 127: // delete
                if (offset > 0) {
                    offset--;
                    puts("\b \b");
                }
                break;
            case '\n':
            case '\r':
                ptr[offset] = '\n';
                putc('\n');
                return offset;
            default:
                putc(c);
                ptr[offset++] = c;
                break;
        }
    }
    return offset;
}

/*
 * write to stdout
 */
ssize_t c_write(int fd, const void *buf, size_t count) {
    size_t nwritten = 0;
    char *ptr = (char *)buf;

    if (((unsigned)buf < SDRAM_START || ((unsigned)buf + count - 1) > SDRAM_END) &&
        ((unsigned)buf + count - 1) > FLASH_END)
        return -EFAULT;

    if (fd != STDOUT_FILENO) {
        return -EBADF;
    }

    while (nwritten != count) {
        putc(ptr[nwritten++]);
    }
    return nwritten;
}

/*
 * wrie in our own handler given the vecotr
 */
int wiring_handler(unsigned vec_num, void *handler_addr) {
//    printf("entering wiring_handler with vec_num = %x\n", vec_num);
    unsigned *vector_addr = (unsigned *)(vec_num * 0x4);
    /*
     * check SWI vector contains the valid format:
     * that is ldr pc, [pc, positive #imm12]
     */
    if (VERBOSE) {
        printf("%x\n", *vector_addr);
    } 
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

//    printf("exiting wiring handler\n");
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


INLINE uint32_t get_OS_time() {
    return TIME_RESOLUTION * sys_time;
}

void set_sleep(unsigned millis) {
    uint32_t deadline = get_OS_time() + millis;
    volatile uint32_t now = get_OS_time();
    while (deadline > now) {
        now = get_OS_time();
    }
}


void init_timer() {
    sys_time = 0;
    if (VERBOSE)
        printf("Entering init timer\n");
    update_timer(TIME_RESOLUTION);
    
    reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
    reg_clear(INT_ICLR_ADDR, 1 << INT_OSTMR_0);     // make it irq
    reg_set(INT_ICMR_ADDR, 1 << INT_OSTMR_0);       // unmask it
}

void update_timer(uint32_t millis) {
    if (VERBOSE)
        printf("Entering update timer\n");
    uint32_t time_in_hz = millis * OSTMR_FREQ_KHZ;
    uint32_t final_time = reg_read(OSTMR_OSCR_ADDR) + time_in_hz;
    reg_write(OSTMR_OSMR_ADDR(0), final_time); // update match reg
}
