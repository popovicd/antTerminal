#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdlib.h>
#include <string.h>

#define MAX_ARGC 5

enum shell_status
{
    SHELL_OK                = 0,
    SHELL_NO_MEMORY         = -1,
    SHELL_TOO_MANY_ARGS     = -2,
    SHELL_COMMAND_NOT_FOUND = -3,
    SHELL_EXIT              = -4,
    SHELL_NEWLINE           = -5,
    SHELL_ARROW             = -6,
};

void shell_init();

int shell_getline(char *line);
int shell_parseline(char ***argv, int *argc, char *line);

int shell_executecmd(char **argv, int argc);
int shell_cmd_ls(char **argv, int argc);
int shell_cmd_cd(char **argv, int argc);
int shell_cmd_pwd(char **argv, int argc);
int shell_cmd_exit(char **argv, int argc);

#endif /* __SHELL_H__ */
