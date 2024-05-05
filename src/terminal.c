#include "terminal.h"

struct terminal_context t_ctx;

void terminal_error(int error)
{
    switch (error)
    {
        case ERR_TCSETATTR:
            perror("Error at setting terminal attributes.");
            exit(1);
        case ERR_TCGETATTR:
            perror("Error at getting terminal attributes.");
            exit(1);
        case ERR_READ:
            perror("Terminal read error.");
            exit(1);
        case ERR_WRITE:
            perror("Terminal write error.");
            exit(1);
        default:
            exit(1);
    }
}

void terminal_init()
{
    memset(t_ctx.buffer, 0, LEN_LINE);
    t_ctx.line_pos = LEN_PROMPT;
    t_ctx.str_len  = 0;
    t_ctx.str_pos  = 0;

    terminal_enable_raw();
}

void terminal_enable_raw()
{
    /* save original terminal settinggs */
    if (tcgetattr(STDIN_FILENO, &t_ctx.orig_termios) == -1)
    {
        terminal_error(ERR_TCGETATTR);
    }

    /* on exit, restore the original terminal */
    atexit(terminal_disable_raw);

    /* set new terminal settings based on the old terminal settings */
    struct termios raw_terminal;
    memcpy(&raw_terminal, &t_ctx.orig_termios, sizeof(struct termios));

    raw_terminal.c_iflag &= ~(ICRNL | IXON);
    raw_terminal.c_oflag &= ~(OPOST);
    raw_terminal.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw_terminal.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw_terminal.c_cflag |= (CS8);
    raw_terminal.c_cc[VMIN]  = 0;
    raw_terminal.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_terminal) == -1)
    {
        terminal_error(ERR_TCGETATTR);
    }
}

void terminal_disable_raw()
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &t_ctx.orig_termios) == -1)
    {
        terminal_error(ERR_TCSETATTR);
    }
}

void terminal_refresh()
{
    if (write(STDOUT_FILENO, "\x1b[?25l", 6) != 6)
    {
        terminal_error(ERR_WRITE);
    }
    if (write(STDOUT_FILENO, "\x1b[2J", 4) != 4)
    {
        terminal_error(ERR_WRITE);
    }
    if (write(STDOUT_FILENO, "\x1b[H", 3) != 3)
    {
        terminal_error(ERR_WRITE);
    }
    if (write(STDOUT_FILENO, "\x1b[?25h", 6) != 6)
    {
        terminal_error(ERR_WRITE);
    }
}

int terminal_get_row()
{
    char buf[32];
    int  i = 0, row = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    {
        terminal_error(ERR_WRITE);
    }

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, buf + i, 1) != 1)
        {
            terminal_error(ERR_READ);
        }
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    sscanf(buf + 2, "%i", &row);

    return row;
}

void terminal_cursor_move(int pos)
{
    int  row;
    char control_seq[32];

    row = terminal_get_row();
    if (row < 0)
    {
        terminal_error(-1); /* default error case */
    }

    snprintf(control_seq, sizeof(control_seq), "\x1b[%d;%dH", row, pos);

    if (write(STDOUT_FILENO, control_seq, strlen(control_seq)) != strlen(control_seq))
    {
        terminal_error(ERR_WRITE);
    }
}

void terminal_prompt(char *prompt)
{
    if (write(STDOUT_FILENO, prompt, strlen(prompt)) != strlen(prompt))
    {
        terminal_error(ERR_WRITE);
    }
}

int terminal_key_process()
{
    int  nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
        {
            terminal_error(ERR_READ);
        }
    }

    if (c == '\x1b')
    {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[')
        {
            switch (seq[1])
            {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
            }
        }
        return '\x1b';
    }
    else
    {
        return c;
    }
}

int terminal_getchar()
{
    int c;

    c = terminal_key_process();

    switch (c)
    {
        /* CTRL + keys to be ignored. If
         * implementing, implementation should
         * be done here. */
        case CTRL_KEY('t'):
        case CTRL_KEY('y'):
        case CTRL_KEY('u'):
        case CTRL_KEY('i'):
        case CTRL_KEY('o'):
        case CTRL_KEY('p'):
        case CTRL_KEY('s'):
        case CTRL_KEY('d'):
        case CTRL_KEY('f'):
        case CTRL_KEY('g'):
        case CTRL_KEY('h'):
        case CTRL_KEY('j'):
        case CTRL_KEY('k'):
        case CTRL_KEY('l'):
        case CTRL_KEY('z'):
        case CTRL_KEY('x'):
        case CTRL_KEY('c'):
        case CTRL_KEY('v'):
        case CTRL_KEY('b'):
        case CTRL_KEY('n'):
            break;
        case CTRL_KEY('q'): /* quit */
            terminal_refresh();
            exit(0);
            break;
        case CTRL_KEY('a'): /* home */
            t_ctx.line_pos = LEN_PROMPT;
            terminal_cursor_move(t_ctx.line_pos);
            break;
        case CTRL_KEY('e'): /* end */
            t_ctx.line_pos = LEN_PROMPT + t_ctx.str_len;
            terminal_cursor_move(t_ctx.line_pos);
            break;
        case ENTER: {
            t_ctx.buffer[t_ctx.str_len + 1] = '\0';

            if (write(STDOUT_FILENO, "\r\n", 2) != 2)
            {
                terminal_error(ERR_WRITE);
            }
            c = '\n';
            break;
        }
        case ARROW_UP:
            break;
        case ARROW_DOWN:
            break;
        case ARROW_LEFT: {
            if (t_ctx.line_pos > LEN_PROMPT)
            {
                t_ctx.line_pos--;
                /* return the current location in buffer */
                t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
            }

            if (t_ctx.line_pos < LEN_PROMPT)
            {
                t_ctx.line_pos = LEN_PROMPT;
                t_ctx.str_pos  = 0;
            }

            terminal_cursor_move(t_ctx.line_pos);
            break;
        }
        case ARROW_RIGHT: {
            if (t_ctx.line_pos - strlen(PROMPT) < t_ctx.str_len)
            {
                t_ctx.line_pos++;
                /* return the current location in buffer */
                t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
            }
            else
            {
                t_ctx.line_pos = t_ctx.str_len + strlen(PROMPT) + 1;
                t_ctx.str_pos  = t_ctx.str_len - 1;
            }

            terminal_cursor_move(t_ctx.line_pos);
            break;
        }
        case BACKSPACE: {
            int line_length, curr_pos, i;

            if (t_ctx.line_pos > LEN_PROMPT)
            {
                t_ctx.line_pos--;
                t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
            }
            else
            {
                t_ctx.str_pos = 0;
                break;
            }

            /* claculate where we at, swap everything from this pont with
             * the rest of the string */
            line_length = t_ctx.str_len + LEN_PROMPT;
            for (i = t_ctx.line_pos; i < line_length; i++)
            {
                curr_pos                               = line_length - i;
                t_ctx.buffer[t_ctx.str_len - curr_pos] = t_ctx.buffer[t_ctx.str_len - curr_pos + 1];
            }

            terminal_cursor_move(t_ctx.line_pos);
            if (write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len], 1) != 1)
            {
                terminal_error(ERR_WRITE);
            }
            terminal_cursor_move(t_ctx.line_pos);

            t_ctx.str_len--;
            /* move everything one char downfront if cursor is in the middle
             * of a t_ctx.buffer string. Afterwards, delete trailing crap
             */
            for (i = t_ctx.line_pos; i < line_length - 1; i++)
            {
                curr_pos = line_length - 1 - i;
                if (write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len - curr_pos], 1) != 1)
                {
                    terminal_error(ERR_WRITE);
                }
            }

            /* erase the last char if backspace presed on the last */
            t_ctx.buffer[t_ctx.str_len] = ' ';

            if (write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len], 1) != 1)
            {
                terminal_error(ERR_WRITE);
            }
            terminal_cursor_move(t_ctx.line_pos);

            break;
        }
        default: { /* any other char */
            /* locks at EOL */
            if (t_ctx.str_len >= LEN_LINE)
            {
                c = EOL;
                break;
            }

            /* somewhere in the middle of a t_ctx.buffer string */
            if (t_ctx.line_pos != LEN_PROMPT + t_ctx.str_len)
            {
                int line_length, curr_pos;

                line_length = LEN_PROMPT + t_ctx.str_len;
                for (int i = line_length + 1; i > t_ctx.line_pos; i--)
                {
                    curr_pos                                   = line_length + 1 - i;
                    t_ctx.buffer[t_ctx.str_len - curr_pos + 1] = t_ctx.buffer[t_ctx.str_len - curr_pos];
                }
                curr_pos                                   = line_length + 1 - t_ctx.line_pos;
                t_ctx.buffer[t_ctx.str_len - curr_pos + 1] = c;

                for (int i = t_ctx.line_pos; i < line_length + 1; i++)
                {
                    curr_pos = line_length - i;
                    if (write(STDOUT_FILENO, &t_ctx.buffer[t_ctx.str_len - curr_pos], 1) != 1)
                        terminal_error(ERR_WRITE);
                }

                t_ctx.line_pos++;
                t_ctx.str_len++;
                t_ctx.str_pos = t_ctx.line_pos - LEN_PROMPT - 2;
                terminal_cursor_move(t_ctx.line_pos);
            }
            /* at the end of a string */
            else
            {
                t_ctx.buffer[t_ctx.str_len] = c;
                t_ctx.str_pos++;
                t_ctx.str_len++;
                t_ctx.line_pos++;

                if (write(STDOUT_FILENO, &c, 1) != 1)
                {
                    terminal_error(ERR_WRITE);
                }
            }

            break;
        }
    }

    return c;
}

int terminal_getline(char *line)
{
    int i, j, c, flag;

    i    = 0;
    flag = 0;

    memset(line, 0, LEN_LINE);

    while (1)
    {
        c = terminal_getchar();

        switch (c)
        {
            case CTRL_KEY('q'):
                return CTRL_Q;
            case '\n': {
                line[t_ctx.str_len + 1] = '\0';
                /* reset intergnals for terminal context */
                if (t_ctx.str_len > 0)
                {
                    memset(t_ctx.buffer, '\0', LEN_LINE);
                    t_ctx.line_pos = LEN_PROMPT;
                    t_ctx.str_pos  = 0;
                    t_ctx.str_len  = 0;
                }
                /* bail out */
                return KEY_NEWLINE;
            }
            case ARROW_UP:
                return KEY_ARROW_UP;
            case ARROW_DOWN:
                return KEY_ARROW_DOWN;
            case ARROW_RIGHT:
            case ARROW_LEFT:
                break;
            case EOL:
                continue;
            case BACKSPACE: {
                /* fix the **line** counter here. When backspace is pressd
                 * we not only need to trace the cursos positions on the screen
                 * but also the index in the line buffer as well. This will have
                 * effect when adding a new char to the EOL after BS was pressed */
                i = i - 2;

                /* on the begging of a line buffer, but backspace was
                 * already pressed. Ignore, nothing to do here.*/
                if (t_ctx.str_pos == 0 && flag == 1)
                {
                    flag = 1;
                    break;
                }

                /* on the begging of a line buffer, but backspace was
                 * not already pressed */
                if (t_ctx.str_pos == 0)
                {
                    for (j = t_ctx.str_pos; j < t_ctx.str_len; j++)
                    {
                        line[j] = line[j + 1];
                    }
                    line[j] = '\0';
                    flag    = 1;
                    break;
                }

                /* at the end of a line buffer, delete the last char in
                 * the buffer */
                if (t_ctx.str_pos == t_ctx.str_len)
                {
                    line[t_ctx.str_len] = '\0';
                    break;
                }

                /* somwhere in the middle of a string buffer. Copy everything
                 * after the current position from the the current postion
                 * till the end */
                for (j = t_ctx.str_pos; j < t_ctx.str_len; j++)
                {
                    line[j] = line[j + 1];
                }
                line[j] = '\0';
                flag    = 0;

                break;
            }
            default: {
                /* end of the string, just append*/
                if (t_ctx.str_pos == t_ctx.str_len)
                {
                    line[i] = c;
                    break;
                }

                /* in the middle of a string, move everything from the
                 * current position one place up. Place a new char in the
                 * current posting. */
                else
                {
                    for (j = t_ctx.str_len + 1; j >= t_ctx.str_pos; j--)
                    {
                        line[j] = line[j - 1];
                    }
                    line[t_ctx.str_pos - 1] = c;
                    line[t_ctx.str_len + 1] = '\0';
                }

                break;
            }
        }
        i++;
    }

    return 0;
}

void terminal_putchar(char c)
{
    if (write(STDOUT_FILENO, &c, 1) != 1)
    {
        terminal_error(ERR_WRITE);
    }
}

void terminal_putstring(char *c)
{
    int i;

    i = 0;
    while (c[i] != '\0')
    {
        terminal_putchar(c[i]);
        i++;
    }
}
