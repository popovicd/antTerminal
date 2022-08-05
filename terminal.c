#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "terminal.h"

struct termios orig_termios;

void terminal_error(int error) {
	switch(error) {
		case TCSETATTR:
			perror("Error at setting terminal attributes: ");
			exit(1);
		case TCGETATTR:
			perror("Error at getting terminal attributes: ");
			exit(1);
		case READ:
			perror("Terminal read error: ");
			exit(1);
		case WRITE:
			perror("Terminal write error: ");
			exit(1);
		deafult:
			exit(1);
	}
}

void terminal_disable_raw() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int terminal_enable_raw() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        terminal_error(TCGETATTR);

	/* Set the new attributes for the terminal */
    struct termios raw_terminal;

	raw_terminal = orig_termios;

	raw_terminal.c_iflag &= ~(ICRNL | IXON);
    raw_terminal.c_oflag &= ~(OPOST);
    raw_terminal.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw_terminal.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw_terminal.c_cflag |= (CS8);
    raw_terminal.c_cc[VMIN] = 0;
    raw_terminal.c_cc[VTIME] = 1;

	/* on exit, restore the original terminal */
	atexit(terminal_disable_raw);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_terminal) == -1)
        terminal_error(TCGETATTR);

    return 0;
}

