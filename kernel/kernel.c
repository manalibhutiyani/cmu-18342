/*
 * kernel.c: Kernel main (entry) function
 *
 * Author: Hsueh-Hung Cheng <xuehung@gmail.com>
 * Date:   10/20/2014 14:38
 */
#include <exports.h>
#include "kernel_helpers.h"

extern int switch_user_app(void *, int, char **);
extern void swi_handler(void *);

int main(int argc, char *argv[]) {

    printf("main: entering");

    if (wiring_SWI_handler(swi_handler) < 0) {
        printf("The instruction in the vector table is unrecognized\n");
        return ERROR;
    }

    return switch_user_app((void *)0xA2000000, argc, argv);
}
