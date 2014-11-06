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

int main(int argc, char** argv)
{
	const char hello[] = "Hello World\r\n";
	write(STDOUT_FILENO, hello, sizeof(hello) - 1);
    while(1);
    //sleep(5000);
/*
    int c = 5;
    while(1) {
        time();
        c--;
        if (c<0) break;
    }
*/
	return 0;
}
