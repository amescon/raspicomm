/// \file
/// \addtogroup api
/// <summary>Checkout the [TMCL Reference](http://www.trinamic.com/tmctechlibcd/Software/TMCL/TMCL_reference.pdf) for specifics.</summary>

#include "statuscode.h"

/// <summary>Steprocker api result codes.</summary>
/// use steprocker_TranslateResult() to convert them into a human readable string 
typedef enum {

  /// <summary>Success - no error</summary>
  SR_SUCCESS,

  /// <summary>RasPiComm was not initialized. Call raspicomm_Init() before calling this function.</summary>
  SR_ERR_RASPICOMM_NOT_INITIALIZED,

  /// <summary>StepRocker was not initialized. Call steprocker_Init() before callling this function.</summary>
  SR_ERR_NOT_INITIALIZED,

  /// <summary>Invalid Parameter - a supplied parameter was invalid.</summary>
  SR_ERR_INVALID_PARAM,

  /// <summary>Function not implemented - please check back later.</summary>
  SR_ERR_NOT_IMPLEMENTED,

  /// <summary>Crc Check failed for the answer.</summary>
  SR_ERR_ANSWER_CRC_FAILED,

  /// <summary>No answer was received within the alloted time.</summary>
  SR_ERR_ANSWER_TIMEOUT,

  /// <summary>Status Code of the answer did not indicate success.</summary>
  SR_ERR_ANSWER_STATUS

} StepRockerResult;

/// <summary>Steprocker axis parameters. Checkout the TMCL References for specifics.</summary>
typedef enum {

  SR_AXIS_PARAM_TARGET_POSITION = 0,
  SR_AXIS_PARAM_ACTUAL_POSITION,
  SR_AXIS_PARAM_TARGET_SPEED,
  SR_AXIS_PARAM_ACTUAL_SPEED,
  SR_AXIS_PARAM_MAX_SPEED,
  SR_AXIS_PARAM_MAX_ACCELERATION,
  SR_AXIS_PARAM_ABS_MAX_CURRENT,
  SR_AXIS_PARAM_STANDBY_CURRENT,
  SR_AXIS_PARAM_TARGET_POSITION_REACHED,
  SR_AXIS_PARAM_REF_SWITCH_STATUS,
  SR_AXIS_PARAM_RIGHT_LIMIT_SWITCH_STATUS,
  SR_AXIS_PARAM_LEFT_LIMIT_SWITCH_STATUS,
  SR_AXIS_PARAM_RIGHT_LIMIT_SWITCH_DISABLE,
  SR_AXIS_PARAM_LEFT_LIMIT_SWITCH_DISABLE,
  SR_AXIS_PARAM_STEPRATE_PRESCALER

} AxisParameter;

/// <summary>Steprocker global parameters. Checkout the TMCL Reference for specifics.</summary>
typedef enum {

  RS_GLOBAL_PARAM_EEPROM_MAGIC = 64,
  RS_GLOBAL_PARAM_BAUD_RATE,
  RS_GLOBAL_PARAM_SERIAL_ADDRESS,
  RS_GLOBAL_PARAM_ASCII_MODE,
  RS_GLOBAL_PARAM_CAN_BIT_RATE = 69,
  RS_GLOBAL_PARAM_CAN_REPLY_ID,
  RS_GLOBAL_PARAM_CAN_ID,
  RS_GLOBAL_PARAM_SYSTEM_ERR_MASK,
  RS_GLOBAL_PARAM_CONFIG_EEPRONN_LOCK,
  RS_GLOBAL_PARAM_ENCODER_INTERFACE,
  RS_GLOBAL_PARAM_TELEGRAM_PAUSE_TIME,
  RS_GLOBAL_PARAM_SERIAL_HOST_ADDRESS,
  RS_GLOBAL_PARAM_AUTO_START_MODE,
  RS_GLOBAL_PARAM_POLL_INTERVAL,
  RS_GLOBAL_PARAM_PORT_FUNCTION_MASK

} GlobalParameter;

/// <summary>Translates the result code into a human readable null terminated string.</summary>
const char* steprocker_TranslateResult(StepRockerResult result);

/// <summary>Initializes the library internals.</summary>
StepRockerResult steprocker_Init(void);

/* motion commands */
StepRockerResult steprocker_RotateLeft(int velocity, StatusCode* pStatus);
StepRockerResult steprocker_RotateRight(int velocity, StatusCode* pStatus);
typedef enum { MVP_ABSOLUTE, MVP_RELATIVE } StepRockerMVPType;
StepRockerResult steprocker_MoveToPosition(StepRockerMVPType type, int value, StatusCode* pStatus);
StepRockerResult steprocker_MotorStop(StatusCode* pStatus);
typedef enum { RFS_START = 0, RFS_STOP = 1, RFS_STATUS = 2 } StepRockerRFSType;
StepRockerResult steprocker_ReferenceSearch(StepRockerRFSType type, StatusCode* pStatus);
StepRockerResult steprocker_SetCoordinate(char number, int value, StatusCode* pStatus);
StepRockerResult steprocker_GetCoordinate(char number, int* pValue, StatusCode* pStatus);
StepRockerResult steprocker_CaptureCoordinate(char number, StatusCode* pAnswerStatus);

/* parameter commands */
StepRockerResult steprocker_SetAxisParameter(AxisParameter number, int value, StatusCode* pStatus);
StepRockerResult steprocker_GetAxisParameter(AxisParameter number, int* pValue, StatusCode* pStatus);
StepRockerResult steprocker_StoreAxisParameterIntoEEPROM(AxisParameter parameter, StatusCode* pStatus);
StepRockerResult steprocker_RestoreAxisParameterFromEEPROM(AxisParameter parameter, StatusCode* pStatus);
StepRockerResult steprocker_SetGlobalParameter(GlobalParameter parameter, int value, StatusCode* pStatus);
StepRockerResult steprocker_GetGlobalParameter(GlobalParameter parameter, int* pValue, StatusCode* pStatus);
StepRockerResult steprocker_StoreGlobalParameterIntoEEPROM(GlobalParameter parameter, StatusCode* pStatus);
StepRockerResult steprocker_RestoreGlobalParameterFromEEPROM(GlobalParameter parameter, StatusCode* pStatus);

/* i/o commands */
StepRockerResult steprocker_SetOutput(char port, int value, StatusCode* pStatus);
StepRockerResult steprocker_GetInput(char port, int* pValue, StatusCode* pStatus);
StepRockerResult steprocker_AccessToExternalSPIDevice();

/* control commands */
StepRockerResult steprocker_JumpAlways();
StepRockerResult steprocker_JumpConditional();
StepRockerResult steprocker_CompareAccumulatorWithConstantValue();
StepRockerResult steprocker_ClearErrorFlags();
StepRockerResult steprocker_CallSubroutine();
StepRockerResult steprocker_ReturnFromSubroutine();
StepRockerResult steprocker_WaitForSpecifiedEvent();
StepRockerResult steprocker_EndOfTMCLProgram();

/* calculation commands */
StepRockerResult steprocker_CalculateUsingAccumulatorAndConstantValue();
StepRockerResult steprocker_CalculateUsingAccumulatorAndXRegister();
StepRockerResult steprocker_CopyAccumulatorToAxisParameter();
StepRockerResult steprocker_CopyAccumulatorToGlobalParameter();
