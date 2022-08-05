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

struct terminal_context {
	char buffer[LEN_LINE];
	int str_len;
	int str_pos;
};

enum error {
	TCSETATTR = 0,
	TCGETATTR,
	READ,
	WRITE,
};

int terminal_enable_raw();
void terminal_disable_raw();
void terminal_error(int error);

int terminal_getchar();
int terminal_key_process();
int terminal_get_row();
int terminal_cursor_move(int pos);

#endif /* __TERMINAL_H__ */
