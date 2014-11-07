/** @file hello.c
 *
 * @brief Prints out Hello world using the syscall interface.
 *
 * Links to libc.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-10-29
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PERIPHERAL_BASE       0x40000000
#define OSTMR_OSCR_ADDR       0x00A00010   /* OS Timer Count Register */
#define OSTMR_FREQ_KHZ        3686.4       /* Oscillator frequency in Khz */

uint32_t reg_read(size_t addr)
{
    return *((volatile uint32_t*)(PERIPHERAL_BASE + addr));
}

int main(int argc, char** argv)
{
	if (argc != 2) {
        printf("Usage: go %s <sleep time in ms>\n", argv[0]);
    }
	//int i;
    //int j = 0;
    volatile uint32_t end_clock = 0;
    uint32_t end_clock_time;
    uint32_t end_time;
    uint32_t start_clock = reg_read(OSTMR_OSCR_ADDR);
    uint32_t start_time = time();


    sleep(atoi(argv[1]));

    end_clock = reg_read(OSTMR_OSCR_ADDR);
    end_time = time();
    end_clock_time = (end_clock - start_clock) / OSTMR_FREQ_KHZ;
    printf("OS clock time interval: %u\n", end_clock_time);
    printf("Sys call time interval: %u\n", end_time - start_time);
	return 0;
}
