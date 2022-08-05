#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#define ENTER         13
#define BACKSPACE    127
#define ARROW_UP    1000
#define ARROW_DOWN  1001
#define ARROW_LEFT  1002
#define ARROW_RIGHT 1003

#define CTRL_KEY(k) ((k) & 0x1f)

#define LEN_LINE    80
#define PROMPT      "gterm > "
#define LEN_PROMPT  strlen(PROMPT) + 1

struct terminal_context {
	char buffer[LEN_LINE];
	int str_len;
	int str_pos;
	int str_start_pos;
};

enum error {
	TCSETATTR = 0,
	TCGETATTR,
	READ,
	WRITE,
};

void terminal_enable_raw();
void terminal_disable_raw();
void terminal_init();
void terminal_error(int error);

void terminal_cursor_move(int pos);
int terminal_getchar();
int terminal_key_process();
int terminal_get_row();

#endif /* __TERMINAL_H__ */
