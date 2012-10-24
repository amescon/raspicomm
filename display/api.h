/// \file
/// \addtogroup Apis

/// <summary>Result Code of a display api call</summary>
typedef enum {

  DI_SUCCESS,
  DI_ERR_RASPICOMM_NOT_INITIALIZED,
  DI_ERR_NOT_INITIALIZED,
  DI_ERR_ALREADY_INITIALIZED,
  DI_ERR_DEV_OPEN,
  DI_ERR_DEV_CONNECT,
  DI_ERR_INVALID_PARAM,
  DI_ERR_NOT_IMPLEMENTED,
  DI_ERR_ANSWER_TIMEOUT,
  DI_ERR_ANSWER_STATUS

} DisplayResult;

/// <summary>Display command send with function display_SendCommand()</summary>
typedef enum {
  DI_CMD_SET_MEMORY_ADDRESSING_MODE = 0x20,

  /// Turn the display off
  DI_CMD_DISPLAY_OFF = 0xAE,

  /// Turn the display on
  DI_CMD_DISPLAY_ON = 0xAF,

  /// Turn the foreground/background color inversion off
  DI_CMD_DISPLAY_INV_OFF = 0xA6,

  /// Turn the foreground/background color inversion on
  DI_CMD_DISPLAY_INV_ON = 0xA7

} DisplayCommand;

/// <summary>Translates the supplied DisplayResult into a human readable null terminated string</summary>
const char* display_TranslateResult(DisplayResult result);

/// <summary>Initializes the Display. raspicomm_Init() must be called before</summary>
/// @param address the i2c address
DisplayResult display_Init(int address);

/// <summary>Does the cleanup</summary>
DisplayResult display_Shutdown(void);

/// <summary>Sends the specified command to the display.</summary>
/// <example>display_SendCommand(DI_CMD_DISPLAY_ON)</example>
/// <param name="cmd">The command to send to the display</param>
DisplayResult display_SendCommand(DisplayCommand cmd);

/// <summary>Prints the character to the display.</summary>
/// consider using display_PrintString() if you have a null terminated string.
/// <param name="c">The character to print at the current screen position.</param>
DisplayResult display_PrintChar(char c);

/// <summary>Prints the null terminated string to the display.</summary>
/// <param name="text">The null terminated string to print at the current screen position.</param>
DisplayResult display_PrintString(char* text);

/// <summary>Moves the cursor to the specifed row and column so that the next display_PrintChar or display_PrintString call prints to the display starting from that location.</summary>
/// <param name="col">The column where the next text will be inserted</param>
/// <param name="row">The row where the next text will be inserteded</param>
DisplayResult display_SetXY(char col, char row);

/// <summary>Clears the screen.</summary>
DisplayResult display_ClearScreen(void);
