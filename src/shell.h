#ifndef __SHELL_H__
#define __SHELL_H__

#define MAX_ARGV 5

enum shell_status
{
    SHELL_OK                = 0,
    SHELL_NO_MEMORY         = -1,
    SHELL_COMMAND_NOT_FOUND = -2,
    SHELL_EXIT              = -3,
    SHELL_NEWLINE           = -4,
    SHELL_ARROW             = -5,
};

int shell_getline(char *line);
int shell_parseline(char ***argv, int *argc, char *line);

int shell_executecmd(char **argv, int argc);
int shell_cmd_ls(char **argv, int argc);
int shell_cmd_cd(char **argv, int argc);
int shell_cmd_pwd(char **argv, int argc);
int shell_cmd_exit(char **argv, int argc);

#endif /* __SHELL_H__ */
