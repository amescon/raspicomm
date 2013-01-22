/// \file
#include <termios.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>Gets a char from the terminal without echo</summary>
char terminal_getchar(void);

#ifdef __cplusplus
}
#endif