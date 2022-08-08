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

#endif /* __SHELL_H__ */
