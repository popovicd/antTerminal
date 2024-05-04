#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "terminal.h"

int shell_cmd_ls(char **argv, int argc)
{
    terminal_putstring("Command:\r\n\t");
    terminal_putstring(argv[0]);
    terminal_putstring("\r\n");

    terminal_putstring("With the following set of arguments:\r\n\t");
    for (int i = 1; i < argc; i++)
    {
        terminal_putstring(argv[i]);
        terminal_putstring("\r\n\t");
    }
    terminal_putstring("\r\n");

    return SHELL_OK;
}

int shell_cmd_cd(char **argv, int argc)
{
    terminal_putstring("Command:\r\n\t");
    terminal_putstring(argv[0]);
    terminal_putstring("\r\n");

    terminal_putstring("With the following set of arguments:\r\n\t");
    for (int i = 1; i < argc; i++)
    {
        terminal_putstring(argv[i]);
        terminal_putstring("\r\n\t");
    }
    terminal_putstring("\r\n");

    return SHELL_OK;
}

int shell_cmd_pwd(char **argv, int argc)
{
    terminal_putstring("Command:\r\n\t");
    terminal_putstring(argv[0]);
    terminal_putstring("\r\n");

    terminal_putstring("With the following set of arguments:\r\n\t");
    for (int i = 1; i < argc; i++)
    {
        terminal_putstring(argv[i]);
        terminal_putstring("\r\n\t");
    }
    terminal_putstring("\r\n");

    return SHELL_OK;
}

int shell_cmd_exit(char **argv, int argc)
{
    terminal_putstring("\r\nExit called\r\n\r\n");
    terminal_disable_raw();

    return SHELL_EXIT;
}
