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

int shell_getline(char *line) {
	int i;
	terminal_init();

	terminal_prompt(PROMPT);
	int rval = terminal_getline(line);

	terminal_putstring(line, strlen(line));

	terminal_putstring("\r\n", 2);

	return rval;
}

int shell_parseline(char ***argv, int *argc, char *line) {
    char *token;
    int argument_count;

    (*argv) = malloc(sizeof(char *) * MAX_ARGV);
    if((*argv) == NULL) {
        return SHELL_NO_MEMORY;
    }

    for(argument_count = 0, token = strtok(line, " "); token ;
		token = strtok(0, " "), argument_count++) {

 	   	(*argv)[argument_count] = malloc(sizeof(char) * strlen(token) + 1);
        if((*argv)[argument_count] == NULL) {
            return SHELL_NO_MEMORY;
        }
        strcpy((*argv)[argument_count], token);
    }

    *argc = argument_count;
    return SHELL_OK;
}


