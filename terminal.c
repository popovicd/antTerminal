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
struct terminal_context t_ctx;

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

void terminal_enable_raw() {
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
}

void terminal_refresh() {
	if(write(STDOUT_FILENO, "\x1b[?25l", 6) != 6)
		terminal_error(WRITE);
    if(write(STDOUT_FILENO, "\x1b[2J", 4) != 4)
		terminal_error(WRITE);
    if(write(STDOUT_FILENO, "\x1b[H", 3) != 3)
		terminal_error(WRITE);
    if(write(STDOUT_FILENO, "\x1b[?25h", 6) != 6)
		terminal_error(WRITE);
}

void terminal_init(struct terminal_context *terminal) {
    memset(terminal->buffer, 0, LEN_LINE);
    terminal->line_pos = LEN_PROMPT;
    terminal->str_len = 0;
    terminal->str_pos = LEN_PROMPT;

    terminal_enable_raw();
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

void terminal_cursor_move(int pos) {
    int row;
    char control_seq[32];

    row = terminal_get_row();
    if(row < 0)
        terminal_error(-1); /* default error case */

    snprintf(control_seq, sizeof(control_seq), "\x1b[%d;%dH", row, pos);

    if(write(STDOUT_FILENO, control_seq, strlen(control_seq)) != strlen(control_seq))
        terminal_error(WRITE);
}

void terminal_prompt(char *prompt) {
    if(write(STDOUT_FILENO, prompt, strlen(prompt)) != strlen(prompt))
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
		/* CTRL + keys to be ignored. If
		 * implementing, implementation should
		 * be done here. */
        case CTRL_KEY('t'):
        case CTRL_KEY('y'):
        case CTRL_KEY('u'):
        case CTRL_KEY('i'):
        case CTRL_KEY('o'):
        case CTRL_KEY('p'):
        case CTRL_KEY('s'):
        case CTRL_KEY('d'):
        case CTRL_KEY('f'):
        case CTRL_KEY('g'):
        case CTRL_KEY('h'):
        case CTRL_KEY('j'):
        case CTRL_KEY('k'):
        case CTRL_KEY('l'):
        case CTRL_KEY('z'):
        case CTRL_KEY('x'):
        case CTRL_KEY('c'):
        case CTRL_KEY('v'):
        case CTRL_KEY('b'):
        case CTRL_KEY('n'):
			break;
        case CTRL_KEY('q'):                 /* quit */
			terminal_refresh();
			exit(0);
            break;
        case CTRL_KEY('a'):                 /* home */
			t_ctx.line_pos = LEN_PROMPT;
			terminal_cursor_move(t_ctx.line_pos);
            break;
        case CTRL_KEY('e'):                 /* end */
            t_ctx.line_pos = LEN_PROMPT + t_ctx.str_len;
            terminal_cursor_move(t_ctx.line_pos);
            break;
        case ENTER:
			t_ctx.buffer[t_ctx.str_len + 1] = '\0';
			if (t_ctx.str_len > 0) {
				memset(t_ctx.buffer, '\0', LEN_LINE);
				t_ctx.line_pos = LEN_PROMPT;
				t_ctx.str_len = 0;
			}

			if(write(STDOUT_FILENO, "\r\n", 2) != 2)
				terminal_error(WRITE);
			c = '\n';
            break;
        case ARROW_UP:
            break;
        case ARROW_DOWN:
            break;
        case ARROW_LEFT:
			if(t_ctx.line_pos > LEN_PROMPT) {
				t_ctx.line_pos--;
				/* return the current location in buffer */
				t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
			}

			if(t_ctx.line_pos < LEN_PROMPT) {
			    t_ctx.line_pos = LEN_PROMPT;
			    t_ctx.str_pos = 0;
			}

			terminal_cursor_move(t_ctx.line_pos);
            break;
        case ARROW_RIGHT:
		   if(t_ctx.line_pos - strlen(PROMPT) < t_ctx.str_len) {
                t_ctx.line_pos++;
                /* return the current location in buffer */
                t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
            }
            else {
                t_ctx.line_pos = t_ctx.str_len + strlen(PROMPT) + 1;
                t_ctx.str_pos = t_ctx.str_len - 1;
            }

            terminal_cursor_move(t_ctx.line_pos);
            break;
        case BACKSPACE:
			int line_length, curr_pos, i;

			if(t_ctx.line_pos > LEN_PROMPT) {
			    t_ctx.line_pos--;
				t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
			}
			else {
				t_ctx.str_pos = 0;
				break;
			}

			/* claculate where we at, swap everything from this pont with
			 * the rest of the string */
			line_length = t_ctx.str_len + LEN_PROMPT;
			for (i = t_ctx.line_pos; i < line_length; i++) {
				curr_pos = line_length - i;
				t_ctx.buffer[t_ctx.str_len - curr_pos] = t_ctx.buffer[t_ctx.str_len - curr_pos + 1];
			}

			/* erase the last char if backspace presed on the last */
			t_ctx.buffer[t_ctx.str_len] = ' ';

			terminal_cursor_move(t_ctx.line_pos);
			if(write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len], 1) != 1)
				terminal_error(WRITE);
			terminal_cursor_move(t_ctx.line_pos);

			t_ctx.str_len--;

			/* move everything one char downfront if cursor is in the middle
			 * of a t_ctx.buffer string. Afterwards, delete trailing crap
			 */
			for (i = t_ctx.line_pos; i < line_length - 1; i++) {
				curr_pos = line_length - 1 - i;
				if(write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len - curr_pos], 1) != 1)
				terminal_error(WRITE);
			}
			t_ctx.buffer[t_ctx.str_len] = ' ';

			if(write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len], 1) != 1)
				terminal_error(WRITE);
			terminal_cursor_move(t_ctx.line_pos);

            break;
		default: 							/* any other char */
            /* locks at EOL */
            if(t_ctx.str_len >= LEN_LINE) {
                c = EOL;
                break;
            }

			/* somewhere in the middle of a t_ctx.buffer string */
			if(t_ctx.line_pos != LEN_PROMPT + t_ctx.str_len) {
				int i, line_length, curr_pos;

				line_length = LEN_PROMPT + t_ctx.str_len;
				for(i = line_length + 1; i > t_ctx.line_pos; i--) {
				    curr_pos =  line_length + 1 - i;
				    t_ctx.buffer[t_ctx.str_len - curr_pos + 1] = t_ctx.buffer[t_ctx.str_len - curr_pos];
				}
				curr_pos = line_length + 1 - t_ctx.line_pos;
				t_ctx.buffer[t_ctx.str_len - curr_pos + 1] = c;

				for (i = t_ctx.line_pos; i < line_length + 1; i++) {
				    curr_pos =  line_length - i;
				    if(write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len - curr_pos], 1) != 1)
				        terminal_error(WRITE);
				}

				t_ctx.line_pos++;
				t_ctx.str_len++;
				t_ctx.str_pos =  t_ctx.line_pos - LEN_PROMPT - 2;
				terminal_cursor_move(t_ctx.line_pos);
			}
			else {
				t_ctx.buffer[t_ctx.str_len] = c;
				t_ctx.str_pos++;
				t_ctx.str_len++;
				t_ctx.line_pos++;

				if(write(STDOUT_FILENO, &c, 1) != 1)
					terminal_error(WRITE);
			}

            break;
    }

    return c;
}
