#ifndef KERNEL_HELPERS
#define KERNEL_HELPERS

int wiring_handler(unsigned, void *);
int C_IRQ_handler(unsigned, unsigned *);
int C_SWI_handler(unsigned, unsigned *);
void restore_handler(unsigned);
ssize_t c_read(int, void *, size_t);
ssize_t c_write(int, const void *, size_t);
unsigned get_OS_time();
void set_sleep(unsigned);

#define SDRAM_START 0xa0000000
#define SDRAM_END 0xa3ffffff

#define UBOOT_HANDLER_ADDR 0x5c0009c0

#endif
