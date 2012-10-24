typedef enum {
	STATUS_SUCCESS = 100,
	STATUS_CMD_LOADED = 101,
	STATUS_ERR_CRC = 1,
	STATUS_ERR_INVALID_CMD = 2,
	STATUS_ERR_WRONG_TYPE = 3,
	STATUS_ERR_INVALID_VALUE = 4,
	STATUS_ERR_EEPROM_LOCKED = 5,
	STATUS_ERR_CMD_NOT_AVAILABLE = 6
} StatusCode;

const char* steprocker_TranslateStatusCode(StatusCode code)
{
	switch (code)
	{
		case STATUS_SUCCESS: return "Successfully executed, no error";
		case STATUS_CMD_LOADED: return "Command loaded into TMCL program EEPROM";
		case STATUS_ERR_CRC: return "Wrong Checksum";
		case STATUS_ERR_INVALID_CMD: return "Invalid Command";
		case STATUS_ERR_WRONG_TYPE: return "Wrong type";
		case STATUS_ERR_INVALID_VALUE: return "Invalid value";
		case STATUS_ERR_EEPROM_LOCKED: return "Configuration EEPROM locked";
		case STATUS_ERR_CMD_NOT_AVAILABLE: return "Command not available";
		default: return "Unknown StatusCode";
	}
}

int steprocker_IsValidStatusCode(StatusCode code)
{
  switch (code)
  {
    case STATUS_SUCCESS:
    case STATUS_CMD_LOADED:
      return 1;

    default: return 0;
  }
}
