/// \file
#include <stdio.h>
#include "../raspicomm/api.h"
#include "../steprocker/api.h"
#include "../display/api.h"
#include <stdlib.h> /* used for exit() */
#include <getopt.h> /* used for argument evaluation in handle_arguments */
#include <unistd.h> /* used for usleep */

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>Available commands</summary>
enum 
{
  /// <summary>specifies the revision of the raspberry pi.</summary>
  CMD_REVISION,

  /// <summary>Start the demo in the interactive mode.</summary>
  CMD_INTERACTIVE,

  /// <summary>Print the help text.</summary>
  CMD_HELP,

  /// <summary>Print the state of the outputs.</summary>
  CMD_GETOUTPUTS,

  /// <summary>Set the outputs to high.</summary>
  CMD_SETOUTPUTS,

  /// <summary>Set the outputs to low.</summary>
  CMD_UNSETOUTPUTS,

  /// <summary>Print the state of the joystick buttons.</summary>
  CMD_GETBUTTONS,

  /// <summary>Set the outputs to high, therefore lighting the LEDs for a short amound of time.</summary>
  CMD_LED_DEMO,

  /// <summary>Print a 'Hello world' text to the LCD display.</summary>
  CMD_LCD_DEMO,

  /// <summary>Enter the interactice type to display mode.</summary>
  CMD_TYPE2DISPLAY,

  /// <summary>Enter the display mode.</summary>
  CMD_DISPLAY_MODE,

  /// <summary>Enter the interactive steprocker mode.</summary>
  CMD_STEPROCKER,

  /// <summary>Enter the interactive steprocker parameter mode.</summary>
  CMD_STEPROCKER_PARAM,

  /// <summary>Quit the demo.</summary>
  CMD_QUIT
} typedef DEMOCMD;

/// <summary>Defines the maximum number of rows in the lcd display.</summary>
#define MAXROWS 8

/// <summary>Defines the maximum number of columns in the lcd display.</summary>
#define MAXCOLUMNS 128

/* forward declaration */
void enter_interactive_mode(void);

#ifdef __cplusplus
}
#endif