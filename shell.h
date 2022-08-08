#ifndef __SHELL_H__
#define __SHELL_H__

#define MAX_ARGV 	5

enum shell_status {
    SHELL_OK = 0,
    SHELL_NO_MEMORY = -1,
    SHELL_COMMAND_NOT_FOUND = -2,
};

int shell_getline(char *line);
int shell_parseline(char ***argv, int *argc, char *line);

int shell_executecmd(char **argv, int argc);
int shell_cmd_ex_one(char **argv, int argc);
int shell_cmd_ex_two(char **argv, int argc);
int shell_cmd_ex_three(char **argv, int argc);

#endif /* __SHELL_H__ */
