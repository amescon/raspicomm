#include "display.h"

extern unsigned char myFont[][8];

typedef struct {

  int IsInitialized;
  int Device;

} internals;

static internals _Internals;

#define I2CDEVICE_REVISION_1 "/dev/i2c-0"
#define I2CDEVICE_REVISION_2 "/dev/i2c-1"

// functions

const char* display_TranslateResult(DisplayResult result)
{
  switch (result)
  {
    case DI_SUCCESS: return "Success - no error";
    case DI_ERR_RASPICOMM_NOT_INITIALIZED: return "Raspicomm was not initialized before calling this function. Consider calling raspicomm_Init() before.";
    case DI_ERR_NOT_INITIALIZED: return "display is not initialized. Consider calling display_Init() before.";
    case DI_ERR_ALREADY_INITIALIZED: return "display is already initialized. Consider calling display_Shutdown() before calling display_Init() again";
    case DI_ERR_DEV_OPEN: return "error occurred while opening the device '/dev/i2c-0'";
    case DI_ERR_DEV_CONNECT: return "error connecting to the device";
    case DI_ERR_INVALID_PARAM: return "the parameter is invalid";
    case DI_ERR_NOT_IMPLEMENTED: return "the function is not implemented";
     
    default: return "Unknown error";
  }
}

DisplayResult display_IsInitialized()
{
  return raspicomm_IsInitialized() != SUCCESS ? 
    DI_ERR_RASPICOMM_NOT_INITIALIZED :
    (_Internals.IsInitialized ? DI_SUCCESS : DI_ERR_NOT_INITIALIZED);
}

DisplayResult display_Init(int address)
{
  DisplayResult result = raspicomm_IsInitialized() == SUCCESS ? 
      DI_SUCCESS : 
      DI_ERR_RASPICOMM_NOT_INITIALIZED;

  // check if we're already initialized
  if (_Internals.IsInitialized)
    result = DI_ERR_ALREADY_INITIALIZED;

  // get the raspberry pi revision
  Revision revision;
  if (result == DI_SUCCESS)
    raspicomm_GetRaspberryPiRevision(&revision);  

  // open the device
  if (result == DI_SUCCESS)
  {
    int device;
    //if ( (device = open("/dev/i2c-0", O_RDWR)) < 0)
    if ( (device = open( revision == REVISION_1 ? 
                                      I2CDEVICE_REVISION_1 : 
                                      I2CDEVICE_REVISION_2, 
                         O_RDWR)) < 0)
      result = DI_ERR_DEV_OPEN;
    else
      _Internals.Device = device;
  }

  // connect to display as i2c slave
  if (result == DI_SUCCESS)
    if (ioctl(_Internals.Device, I2C_SLAVE, address) < 0)
      result = DI_ERR_DEV_CONNECT;

  // mark the display as initialized
  if (result == DI_SUCCESS)
    _Internals.IsInitialized = 1;

  return result;
}

DisplayResult display_Shutdown()
{
  // close the device
  if (_Internals.Device)
  {
    close(_Internals.Device);
    _Internals.Device = 0;
  }
}

DisplayResult display_SendChar(char c)
{
  DisplayResult result = display_IsInitialized();

  if (result == DI_SUCCESS)
  {
    char buffer[2]; 
    buffer[0] = 0x40;
    buffer[1] = c;

    write (_Internals.Device, buffer, 2);
  }

  return result;
}

DisplayResult display_SendCommand(DisplayCommand cmd)
{
  DisplayResult result = display_IsInitialized();
  if (result == DI_SUCCESS)
  {
    char buffer[2]; 
    buffer[0] = 0x80; 
    buffer[1] = cmd;

    write (_Internals.Device, buffer, 2);
  }
  return result;
}

DisplayResult display_PrintChar(char c)
{
  DisplayResult result = display_IsInitialized();
  if (result == DI_SUCCESS)
  {
    int i;
    for (i = 0; i < 8; i++)
      display_SendChar(myFont[c-0x20][i]);
  }
  return result;
}

DisplayResult display_PrintString(char* text)
{
  DisplayResult result = display_IsInitialized();
  if (result == DI_SUCCESS)
  {
    char c;
    while ( (c = *(text++)) != 0)
      display_PrintChar(c);
  }
  return result;

}

DisplayResult display_SetXY(char col, char row)
{
  DisplayResult result = display_IsInitialized();
  if (result == DI_SUCCESS)
  {
    display_SendCommand(0xb0+row);                //set page address
    display_SendCommand(0x00+(8*col&0x0f));       //set low col address
    display_SendCommand(0x10+((8*col>>4)&0x0f));  //set high col address
  }
  return result;
}

DisplayResult display_ClearScreen()
{
  DisplayResult result = display_IsInitialized();
  if (result == DI_SUCCESS)
  {
    unsigned char i,k;
    for(k=0;k<8;k++)
    {	
	    display_SetXY(0, k);    
	    {
	      for(i=0;i<128;i++)     //clear all COL
	      {
		      display_SendChar(0);         //clear all COL
		      //delay(10);
	      }
	    }
    }
  }
  return result;
}