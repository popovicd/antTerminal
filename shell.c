#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "terminal.h"
#include "shell.h"

extern struct terminal_context t_ctx;

void shell_getline(char *line) {
	int i;
	terminal_init();

	while(1) {
		terminal_prompt(PROMPT);
		terminal_getline(line);

		terminal_putstring(line, strlen(line));

		terminal_putstring("\r\n", 2);
	}
}
