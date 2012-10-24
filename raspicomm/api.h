/// \file raspicomm/api.h
/// \addtogroup api
/// <summary>
/// This file contains the main raspicomm api.
///
/// Start by using init function and evaluating it's result.
///
/// <warning>Example usage:
///
/// ApiResult result = raspicomm_Init();
///
/// printf( "Calling raspicomm_Init(): %i %s\n", result, raspicomm_TranslateResult(result));
/// </summary>
/// \warning You need superuser privileges to execute raspicomm api calls.
#include <time.h>
#include "apiresult.h"
#include "button.h"
#include "output.h"
#include "max3140.h"
#include "revision.h"

/// <summary>initializes the RasPiComm</summary>
/// Must be the first function to call
extern ApiResult raspicomm_Init(Revision revision);

/// <summary>Returns SUCCESS if it is already initialized, otherwise ERR_NOT_INITIALIZED</summary>
extern ApiResult raspicomm_IsInitialized(void);

/// <summary>Sets the caller allocated Revision with the used revision</summary>
/// <param name="revision">pointer to the revision</revision>
/// <return>Returns ERR_NOT_INITIALIZED if the raspicomm_Init() was not called before and ERR_INVALID_PAAM if revision is NULL, otherwise SUCCESS.</returns>
extern ApiResult raspicomm_GetRaspberryPiRevision(Revision* revision);

/// <summary>Translates the ApiResult into a human readable error string</summary>
/// <param name="error">The result to translate</param>
extern const char* raspicomm_TranslateResult(ApiResult error);

/// <summary>Fills the caller allocated Buttons with the current state</summary>
/// <param name="buttons">pointer to a Buttons struct</param>
extern ApiResult raspicomm_GetButtons(Buttons* buttons);

/// <summary>Sets the specified outputs to high</summary>
/// <param name="output">Output flags to set</param>
/// <example>e.g. raspicomm_SetOutputs( LED1 | LED2 )</example>
extern ApiResult raspicomm_SetOutputs(Outputs output);

/// <summary>Clears the specified outputs</summary>
/// <param name="output">Output flags to clear</param>
/// <example>e.g. raspicomm_ClearOutputs( LED1 | LED2 )</example>
extern ApiResult raspicomm_ClearOutputs(Outputs output);

/// <summary>Gets the state the output was last set to. 0..low 1..high</summary>
/// <param name="output">Output flags to get</param>
/// <example>e.g. raspicomm_GetOutputs( LED1 | LED2 )</example>
extern ApiResult raspicomm_GetOutputs(Outputs* output);

/// <summary>Sets the time of the realtimeclock</summary>
/// <param name="time">The time to which the clock is set</param>
extern ApiResult raspicomm_SetTime(time_t time);

/// <summary>Fills the caller allocated time struct with the time of the realtimeclock
/// Consider using the standard C library way, e.g. "time_t t = time(NULL);" instead</summary>
extern ApiResult raspicomm_GetTime(time_t* time);

/// <summary>Shuts the SPI polling thread down</summary>
extern ApiResult raspicomm_Shutdown(void);

/// <summary>Sets the baudrate, stop bits, data bits of the rs485.</summary>
/// <param name="baudrate">The baudrate to use for communication.</param>
/// <param name="databits">The databits setting to use for communication.</param>
/// <param name="stopbits">The stopbits setting to use for communication.</param>
/// <param name="parity">The parity setting to use for communication.</param>
/// <param name="recv_callback">The callback method that will be invoked when a character is received.</param>
extern ApiResult raspicomm_InitRs485(Baudrate baudrate, Databits databits, Stopbits stopbits, Parity parity, void (*recv_callback)(char));

/// <summary>Sends the supplied bytes to the spi interface.</summary>
/// <param name="str">The bytes to write to the spi interface.</param>
/// <param name="length">The length of the bytes to sent to the spi interface.</param>
/// <param name="sentBytes">Is set to the number of bytes sent</param>
extern ApiResult raspicomm_spi_write(char* str, int length, int* sentBytes);