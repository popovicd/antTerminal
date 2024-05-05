#include "shell.h"
#include "terminal.h"

int main(void)
{
    int rval;

    int    argc;
    char **argv;
    char   line[LEN_LINE] = { 0 };

    shell_init();

    while (1)
    {
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

        rval = shell_parseline(&argv, &argc, line);
        if (rval == SHELL_TOO_MANY_ARGS)
        {
            terminal_putstring("Too many args, accepting command + 4 args max\r\n");
        }

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
            exit(0);
        }
    }

    return 0;
}
