#include "steprocker.h"

// translate the suppliled result into a human readable string
const char* steprocker_TranslateResult(StepRockerResult result)
{
  switch (result)
  {
    case SR_SUCCESS: return "Success - No Error";
    case SR_ERR_RASPICOMM_NOT_INITIALIZED: return "raspicomm was not initialized before calling a steprocker function. Consider calling raspicomm_init() before calling this function.";
    case SR_ERR_INVALID_PARAM: return "A parameters supplied to this function was invalid";
    case SR_ERR_NOT_IMPLEMENTED: return "Function not implemented";
    case SR_ERR_ANSWER_CRC_FAILED: return "Crc check failed for the answer";
    case SR_ERR_ANSWER_TIMEOUT: return "No answer was received within the alloted time";
    case SR_ERR_ANSWER_STATUS: return "Status Code of the answer did not indicate success";
  }
}

StepRockerResult steprocker_IsInitialized()
{
	ApiResult result = raspicomm_IsInitialized();

	return result == SUCCESS ? SR_SUCCESS : SR_ERR_RASPICOMM_NOT_INITIALIZED;
}

void steprocker_ReceiveCallback(char c)
{
  printf("%X ", c);

  // store the character in the receiveBuffer
  if (_Internals.receiveBufferIndex < RECEIVEBUFFER_LENGTH)
  {
    _Internals.receiveBuffer[_Internals.receiveBufferIndex++] = c;

    // check if we've received the full answer
    if (_Internals.receiveBufferIndex == RECEIVEBUFFER_LENGTH)
      printf("\n");
  }

}

// initialize the steprocker. Must be the first steprocker function call.
StepRockerResult steprocker_Init(void)
{	
  StepRockerResult result = SR_SUCCESS;

  // configure the Rs485 port for steprocker usage
  raspicomm_InitRs485(BAUDRATE_9600, DATABITS_8, STOPBITS_ONE, PARITY_OFF, steprocker_ReceiveCallback);  

	return result;
}

void steprocker_SendCommand(const struct tmclcmd* ptmclcmd)
{
	//sizeof( (*ptmclcmd)) = 12 - TODO maybe we should use a union instead of hardcoded value?
	// Write ( (char*)ptmclcmd, sizeof( (*ptmclcmd)));
	raspicomm_spi_write( (char*)ptmclcmd, 9, NULL );
}

// returns the checksum for the supplied cmd
unsigned char steprocker_CalcChecksum(char *cmd)
{
	unsigned char checksum = 0, i=0;

	for(i = 0; i < 8; i++)
      checksum += *((cmd)+i);
    
  return checksum;
}

// updates the supplied tmclcmd struct with the parameters
void steprocker_UpdateCommand(struct tmclcmd *cmd, 
															unsigned char moduleAddress, 
															unsigned char command, 
															unsigned char type, 
															unsigned char motor, 
															long value)
{
	cmd->moduleAddress = moduleAddress;
	cmd->command = command;
	cmd->type = type;
	cmd->motor = motor;
	*(((char*)&cmd->value)+0) = value >> 24;
	*(((char*)&cmd->value)+1) = value >> 16;
	*(((char*)&cmd->value)+2) = value >> 8;
	*(((char*)&cmd->value)+3) = value & 0xff;

	cmd->checksum = steprocker_CalcChecksum((char*) cmd);
}


typedef enum {
  REPLY_REPLY_ADDRESS = 0,
  REPLY_MODULE_ADDRESS = 1,
  REPLY_STATUS = 2,
  REPLY_COMMAND = 3,
  REPLY_VALUE = 4,
  REPLY_CHECKSUM = 8
} ReplyOffset;

StepRockerResult steprocker_UpdateAndSendCommand(unsigned char command, 
																		             unsigned char type, 
																		             long value,
                                                 StatusCode* pAnswerStatus,
                                                 int* pAnswerValue)
{
	// ModuleAddress = 1, Motor = 0

  StepRockerResult result = steprocker_IsInitialized();

  if (result == SR_SUCCESS)
  {
    // reset the buffer
    memset(_Internals.receiveBuffer, 0, RECEIVEBUFFER_LENGTH);
    _Internals.receiveBufferIndex = 0;

	  steprocker_UpdateCommand(&_Internals.cmd, 1, command, type, 0, value);
	  steprocker_SendCommand(&_Internals.cmd);
  }

  // block & wait for an answer
  if (result == SR_SUCCESS)
  {    
    int maxTime = 1000000; int time = 0; int sleepTime = 1000;
    // wait until the receive buffer is full or 1 second elapsed
    while (time <= maxTime && _Internals.receiveBufferIndex < RECEIVEBUFFER_LENGTH)
    {
      time += sleepTime;
      usleep(sleepTime);
    }
      
    // test for timeout
    if (time > maxTime)
      result = SR_ERR_ANSWER_TIMEOUT;
  }
  
  // test the checksum
  if (result == SR_SUCCESS)
  {
    char crc = steprocker_CalcChecksum(_Internals.receiveBuffer);

    if (crc != _Internals.receiveBuffer[REPLY_CHECKSUM])
      result = SR_ERR_ANSWER_CRC_FAILED;
  }

  // validate the statuscode
  char status = 0;  
  if (result == SR_SUCCESS)
  {
    status = _Internals.receiveBuffer[REPLY_STATUS];

    if (pAnswerStatus != NULL)
      (*pAnswerStatus) = status;

    if (!steprocker_IsValidStatusCode(status))
      result = SR_ERR_ANSWER_STATUS;

    if (pAnswerValue != NULL)
    {
      // switch the byte order
      //*pAnswerValue = *(int*)(_Internals.receiveBuffer + REPLY_VALUE);
      char* p = (char*)pAnswerValue;
      int i;
      for (i = 0; i < 4; i++)
        p[i] = *(_Internals.receiveBuffer + REPLY_VALUE + 3 - i);
    }
  }

  return result;
}

// steprocker api functions

// motion commands
StepRockerResult steprocker_RotateRight(int velocity, StatusCode* pStatus)
{	
	return steprocker_UpdateAndSendCommand(SR_CMD_ROR, 0, velocity, pStatus, NULL);
}

StepRockerResult steprocker_RotateLeft(int velocity, StatusCode* pStatus)
{	
	return steprocker_UpdateAndSendCommand(SR_CMD_ROL, 0, velocity, pStatus, NULL);
}

StepRockerResult steprocker_MotorStop(StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_MST, 0, 0, pStatus, NULL);
}

StepRockerResult steprocker_MoveToPosition(StepRockerMVPType type, int value, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_MVP, type, value, pStatus, NULL);
}

StepRockerResult steprocker_ReferenceSearch(StepRockerRFSType type, StatusCode* pStatus)
{
	return steprocker_UpdateAndSendCommand(SR_CMD_RFS, type, 0, pStatus, NULL);
}

StepRockerResult steprocker_SetCoordinate(char number, int value, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_SCO, number, value, pStatus, NULL);
}

StepRockerResult stepRocker_GetCoordinate(char number, int* pValue, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_GCO, number, 0, pStatus, pValue);
}

StepRockerResult steprocker_CaptureCoordinate(char number, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_CCO, number, 0, pStatus, NULL);
}

// parameter commands
StepRockerResult steprocker_SetAxisParameter(AxisParameter parameter, int value, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_SAP, parameter, value, pStatus, NULL);
}

StepRockerResult steprocker_GetAxisParameter(AxisParameter parameter, int* pValue, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_GAP, parameter, 0, pStatus, pValue);
}

StepRockerResult steprocker_StoreAxisParameter(AxisParameter parameter, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_STAP, parameter, 0, pStatus, NULL);
}

StepRockerResult steprocker_RestoreAxisParameter(AxisParameter parameter, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_RSAP, parameter, 0, pStatus, NULL);
}

StepRockerResult steprocker_SetGlobalParameter(GlobalParameter parameter, int value, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_SGP, parameter, value, pStatus, NULL);
}

StepRockerResult steprocker_GetGlobalParameter(GlobalParameter parameter, int* pValue, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_GGP, parameter, 0, pStatus, pValue);
}

StepRockerResult steprocker_StoreGlobalParameterIntoEEPROM(GlobalParameter parameter, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_STGP, parameter, 0, pStatus, NULL);
}

StepRockerResult steprocker_RestoreGlobalParameterFromEEPROM(GlobalParameter parameter, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_RSGP, parameter, 0, pStatus, NULL);
}

// i/o commands
StepRockerResult steprocker_SetOutput(char port, int value, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_SIO, port, value, pStatus, NULL);
}

StepRockerResult steprocker_GetInput(char port, int* pValue, StatusCode* pStatus)
{
  return steprocker_UpdateAndSendCommand(SR_CMD_GIO, port, 0, pStatus, pValue);
}

// control commands