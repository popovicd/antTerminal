## antTerminal

Simple terminal implementation with a demo shell on top of it.

It is quite simple: implements basic character handling, left/right arrow movement in the current line, blocking at the EOL limit, deleting characters,
inserting characters and a blueprint for the implementation of CTRL characters/sequences.  These are used by the demo shell which implements a
command line parsing and command execution example. Based on this implementation, project can be expanded in various different ways.

I got the idea to write this implementation in my other projects where I had to use a shell like approach for communication between different components,
such as a container and a host over a unix socket rather than STDOUT.

### Basic remarks

Terminal code is contained in `terminal.c` and `terminal.h` at `src` location. These provide basic operations for character handling and terminal setup. The
`shell.c` and `shell.h` provide a simple implentation of shell parser. As it can be noticed, terminal is accessed directly by calling the terminal functions
such as `void terminal_putstring(char *c)` directly but in pratice, these function should be wrapped around with a "library" like functions such as `printf`.

### Building and testing

To build a project, cerate a `build` directory in the root project directory and `cd` into it:

```
mkdir build
cd build
```

Invoke `cmake` from there flowed by `make`:

```
cmake ..
make
```

The binary can then be started from the same location:

```
./antTerminal
```

Once in the shell, four demo commands are supported: `ls`, `cd`, `pwd` and `exit`. They do nothing at all and just serve as a demo commands for testing the
basic behavior of the shell and the terminal. Max number of arguments passed to any of these is four (five with the command) and passing more then four args will cut
them off on four.

To test, run:

```
term > ls arg1 arg2 arg3 arg4
```

Passing an unregistered command will trigger `unknown command` response. Current max line length is set by default to 80 characters. Try printing out a character 80 times
and navigate with arrows over the line and delete/modify the characters to test the behavior.

For exit, type `exit` or press `CTRL+q` which will exit, clear and reset the terminal back to the original terminal.
