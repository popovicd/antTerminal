#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "terminal.h"

extern struct terminal_context t_ctx;

char *shell_commands[] = { "ls", "cd", "pwd", "exit" };

int (*command_functions[])(char **argv, int argc) = { &shell_cmd_ls, &shell_cmd_cd, &shell_cmd_pwd, &shell_cmd_exit };

int shell_getline(char *line)
{
    int rval = SHELL_OK;

    terminal_init();

    rval = terminal_getline(line);
    if ((rval == KEY_NEWLINE) && (strcmp(line, "")) == 0)
    {
        rval = SHELL_NEWLINE;
    }
    if ((rval == KEY_ARROW_UP) || (rval == KEY_ARROW_DOWN))
    {
        rval = SHELL_ARROW;
    }

    return rval;
}

int shell_parseline(char ***argv, int *argc, char *line)
{
    int rval = SHELL_OK;

    int   argument_count;
    char *token;

    (*argv) = malloc(sizeof(char *) * MAX_ARGV);
    if ((*argv) == NULL)
    {
        rval = SHELL_NO_MEMORY;
        goto end;
    }

    for (argument_count = 0, token = strtok(line, " "); token; token = strtok(0, " "), argument_count++)
    {

        (*argv)[argument_count] = malloc(sizeof(char) * strlen(token) + 1);
        if ((*argv)[argument_count] == NULL)
        {
            rval = SHELL_NO_MEMORY;
            goto end;
        }
        strcpy((*argv)[argument_count], token);
    }

    *argc = argument_count;

end:
    return rval;
}

int shell_executecmd(char **argv, int argc)
{
    int rval = SHELL_COMMAND_NOT_FOUND;

    for (int i = 0; i < sizeof(shell_commands) / sizeof(char *); i++)
    {
        if (strcmp(argv[0], shell_commands[i]) == 0)
        {
            rval = (*command_functions[i])(argv, argc);
            break;
        }
    }

    return rval;
}
