#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "terminal.h"

int shell_cmd_ex_one(char **argv, int argc) {
	int i;

	terminal_putstring("\r\nThe following was input:\r\n\r\n");

	terminal_putstring("Command:\r\n\t");
	terminal_putstring(argv[0]);
	terminal_putstring("\r\n");

	terminal_putstring("With the following set of arguments:\r\n\t");
	for(i = 1; i < argc; i++) {
		terminal_putstring(argv[i]);
		terminal_putstring("\r\n\t");
	}
	terminal_putstring("\r\n");

	return SHELL_OK;
}


int shell_cmd_ex_two(char **argv, int argc) {
	int i;

	terminal_putstring("\r\nThe following was input:\r\n\r\n");

	terminal_putstring("Command:\r\n\t");
	terminal_putstring(argv[0]);
	terminal_putstring("\r\n");

	terminal_putstring("With the following set of arguments:\r\n\t");
	for(i = 1; i < argc; i++) {
		terminal_putstring(argv[i]);
		terminal_putstring("\r\n\t");
	}
	terminal_putstring("\r\n");

	return SHELL_OK;
}

int shell_cmd_ex_three(char **argv, int argc) {
	int i;

	terminal_putstring("\r\nThe following was input:\r\n\r\n");

	terminal_putstring("Command:\r\n\t");
	terminal_putstring(argv[0]);
	terminal_putstring("\r\n");

	terminal_putstring("With the following set of arguments:\r\n\t");
	for(i = 1; i < argc; i++) {
		terminal_putstring(argv[i]);
		terminal_putstring("\r\n\t");
	}
	terminal_putstring("\r\n");

	return SHELL_OK;
}
