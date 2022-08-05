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

int terminal_get_row() {
    char buf[32];
    int i = 0, row = 0;

    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        terminal_error(WRITE);

    while (i < sizeof(buf) - 1) {
        if(read(STDIN_FILENO, buf + i, 1) != 1) {
			terminal_error(READ);
		}
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    sscanf(buf + 2, "%i", &row);

    return row;
}

int terminal_cursor_move(int pos) {
    int row;
    char control_seq[32];

    row = terminal_get_row();
    if(row < 0)
        terminal_error(-1); /* default error case */

    snprintf(control_seq, sizeof(control_seq), "\x1b[%d;%dH", row, pos);

    if(write(STDOUT_FILENO, control_seq, strlen(control_seq)) != strlen(control_seq))
        terminal_error(WRITE);
}

int terminal_key_process() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            terminal_error(READ);
    }

    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return '\x1b';

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
            }
        }
        return '\x1b';
    }
    else {
        return c;
    }
}

int terminal_getchar() {
    int c;

    c = terminal_key_process();

    switch (c) {
        case CTRL_KEY('q'):                 /* quit */
            break;
        case CTRL_KEY('a'):                 /* home */
            break;
        case CTRL_KEY('e'):                 /* end */
            break;
        case ENTER:
            break;
        case ARROW_UP:
            break;
        case ARROW_DOWN:
            break;
        case ARROW_LEFT:
            break;
        case ARROW_RIGHT:
            break;
        case BACKSPACE:
            break;
		default: 							/* any other char */

            break;
    }

    return c;
}
