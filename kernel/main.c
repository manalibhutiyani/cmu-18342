#include <exports.h>

#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include "kernel_helpers.h"

#define USER_APP_ADDR 0xA0000000

extern int switch_user_app(void *, int, char **);
extern void swi_handler(void *);
extern void irq_handler(void *);

uint32_t global_data;
int version = 5;

int kmain(int argc, char** argv, uint32_t table)
{
    printf("version = %d\n", version);
    void *user_app_addr = (void *)USER_APP_ADDR;

	app_startup(); /* bss is valid after this point */
    global_data = table;

    /* wiring SWI handler */
    if (wiring_handler(EX_SWI, swi_handler) < 0) {
        printf("The instruction in the vector table is unrecognized\n");
        return KERNEL_ERROR;
	}
    /* wiring IRQ handler */
    if (wiring_handler(EX_IRQ, irq_handler) < 0) {
        printf("The instruction in the vector table is unrecognized\n");
        return KERNEL_ERROR;
	}
	switch_user_app(user_app_addr, argc, argv);

	return 0;
}
