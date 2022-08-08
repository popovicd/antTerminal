#include <stdlib.h>
#include <string.h>

#include "terminal.h"
#include "shell.h"

extern struct terminal_context t_ctx;

char *shell_commands[] = {
	"cmd-example-1",
	"cmd-example-2",
	"cmd-example-3",
	"cmd-exit"
};

int (*command_functions[]) (char **argv, int argc) = {
//	&shell_cmd_ex_one,
//	&shell_cmd_ex_two,
//	&shell_cmd_ex_three
};

int shell_getline(char *line) {
	int rval, i;
	terminal_init();

	terminal_prompt(PROMPT);
	rval = terminal_getline(line);

	terminal_putstring(line);

	terminal_putstring("\r\n");

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

int shell_executecmd(char **argv, int argc) {
	int rval, i;

    for (i = 0; i < sizeof(shell_commands) / sizeof(char *); i++) {
        if (strcmp(argv[0], shell_commands[i]) == 0) {
            rval = (*command_functions[i])(argv, argc);
            return rval;
        }
    }

    return SHELL_COMMAND_NOT_FOUND;
}
