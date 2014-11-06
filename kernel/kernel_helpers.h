#ifndef KERNEL_HELPERS
#define KERNEL_HELPERS

int wiring_handler(unsigned, void *);
int C_IRQ_handler(unsigned, unsigned *);
int C_SWI_handler(unsigned, unsigned *);
void restore_handler(unsigned);
ssize_t c_read(int, void *, size_t);
ssize_t c_write(int, const void *, size_t);
uint32_t get_OS_time();
void set_sleep(unsigned);
void init_timer();
void update_timer(uint32_t);

#define SDRAM_START	0xa0000000	 /* SDRAM (64 MB) start address */
#define SDRAM_END	0xa3ffffff	 /* SDRAM (64 MB) end address */
#define FLASH_START	0x00000000	 /* StrataFlash start address */
#define FLASH_END 	0x00ffffff	 /* StrataFlash end address */

#define UBOOT_HANDLER_ADDR 0x5c0009c0
#define KERNEL_ERROR 	   0x0badc0de   /* unrecognized instruction error ret */
#define VERBOSE  0

#endif
