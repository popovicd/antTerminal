#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#define ENTER         13
#define BACKSPACE    127
#define ARROW_UP    1000
#define ARROW_DOWN  1001
#define ARROW_LEFT  1002
#define ARROW_RIGHT 1003
#define EOL 		1005

#define CTRL_KEY(k) ((k) & 0x1f)

#define LEN_LINE    80
#define PROMPT      "term > "
#define LEN_PROMPT  strlen(PROMPT) + 1

struct terminal_context {
	char buffer[LEN_LINE];
	int str_len;
	int str_pos;
	int line_pos;
};

enum error {
	TCSETATTR = 0,
	TCGETATTR,
	READ,
	WRITE,
};

void terminal_enable_raw();
void terminal_disable_raw();
void terminal_refresh();
void terminal_init();
void terminal_error(int error);

void terminal_prompt(char *prompt);
void terminal_cursor_move(int pos);
void terminal_getline(char *line);
void terminal_putchar(char c);
void terminal_putstring(char *c, int len);
int terminal_getchar();
int terminal_key_process();
int terminal_get_row();

#endif /* __TERMINAL_H__ */
