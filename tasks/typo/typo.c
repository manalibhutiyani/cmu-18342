/** @file typo.c
 *
 * @brief Echos characters back with timing data.
 *
 * Links to libc.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LINE 255
#define V 0

int main(int argc, char** argv)
{
	/* Add code here -- put your names at the top. */
	unsigned long start = 0;
	unsigned long now = 0;
	unsigned long elapsed;
	unsigned long decimal;
	char buf[MAX_LINE];
	char sbuf[MAX_LINE];
	ssize_t readret;
	//printf("xxx");
	while (1) {

		write(STDOUT_FILENO, ">", 1);
		if (V) {
			sprintf(sbuf, "start_before_call_time:%ld\n", start);
			write(STDOUT_FILENO, sbuf, strlen(sbuf));
		}
		start = time();
		if (V) {
			sprintf(sbuf, "start_after_call_time:%ld\n", start);
			write(STDOUT_FILENO, sbuf, strlen(sbuf));
		}
		readret = read(STDIN_FILENO, buf, MAX_LINE);
		now = time();
		if (V) {
			sprintf(sbuf, "now_after_call_time:  %ld\n", now);
			write(STDOUT_FILENO, sbuf, strlen(sbuf));
		}
		elapsed = (now - start) / 1000;
		decimal = ((now - start) % 1000) / 100;
		write(STDOUT_FILENO, buf, readret);
		
		sprintf(sbuf, "\n%lu.%lus\n", elapsed, decimal);
		write(STDOUT_FILENO, sbuf, strlen(sbuf));
	}
	return 0;
}
