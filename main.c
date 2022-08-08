#include <stdlib.h>

#include "terminal.h"
#include "shell.h"

extern struct terminal_context t_ctx;

int main() {
    int i, rval;

    char **argv;
    int argc;
    char line[LEN_LINE]  = {0};

    while(1) {
        rval = shell_getline(line);
        if(rval == KEY_NEWLINE)
            continue;

        shell_parseline(&argv, &argc, line);

        rval = shell_executecmd(argv, argc);
        if(rval == SHELL_COMMAND_NOT_FOUND) {
            terminal_putstring("Command not found\r\n");
        }

        for(i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);
    }

    return 0;
}
