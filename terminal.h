#ifndef __TERMINAL_H__
#define __TERMINAL_H__


/*
struct terminal_context term_ctx {

}
*/

enum error {
	TCSETATTR = 0,
	TCGETATTR,
	READ,
	WRITE,
};

int terminal_enable_raw();
void terminal_disable_raw();
void terminal_error(int error);

#endif /* __TERMINAL_H__ */
