#include "terminal.h"

static struct termios _old, _new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &_old); /* grab old terminal i/o settings */
  _new = _old; /* make new settings same as old settings */
  _new.c_lflag &= ~ICANON; /* disable buffered i/o */
  _new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &_new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void restoreTermios(void) 
{
  tcsetattr(0, TCSANOW, &_old);
}

char terminal_getchar(void)
{
	char c;
	initTermios(0);
	c = getchar();
	restoreTermios();
	return c;
}