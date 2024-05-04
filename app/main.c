#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "terminal.h"

extern struct terminal_context t_ctx;

int main(void)
{
    int rval;

    int    argc;
    char **argv;
    char   line[LEN_LINE] = { 0 };

    while (1)
    {
        terminal_prompt(PROMPT);
        rval = shell_getline(line);

        if (rval == SHELL_ARROW)
        {
            terminal_putstring("\r\nArrow up or down pressed\r\n");
            continue;
        }

        if ((rval == SHELL_NEWLINE) && (strcmp(line, "")) == 0)
        {
            continue;
        }

        shell_parseline(&argv, &argc, line);

        rval = shell_executecmd(argv, argc);
        if (rval == SHELL_COMMAND_NOT_FOUND)
        {
            terminal_putstring("Command not found\r\n");
        }

        for (int i = 0; i < argc; i++)
        {
            free(argv[i]);
        }
        free(argv);

        if (rval == SHELL_EXIT)
        {
            break;
        }
    }

    return 0;
}
