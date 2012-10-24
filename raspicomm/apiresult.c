#include "apiresult.h"

// Translates the ApiResult code into a human readable string
const char* raspicomm_TranslateResult(ApiResult error)
{
  switch (error)
  {
    case SUCCESS: return "Success - No Error";
    case ERR_ALREADY_INITIALIZED: return "Already initialized - init() was already called before";
    case ERR_ALLOC: return "Memory allocation error";
    case ERR_MMAP: return "Error calling mmap";
    case ERR_NOT_INITIALIZED: return "Not initialized - init() was not called before";
    case ERR_NOT_IMPLEMENTED: return "Function not yet implemented";
    case ERR_INVALID_PARAM: return "Supplied function parameter is invalid";
    case ERR_OPEN_EXPORT_DEV: return "Error opening the gpio export device (/sys/class/gpio/export)";
    case ERR_WRITE_EXPORT_DEV: return "Error writing to the gpio export device (/sys/class/gpio/export)";
    case ERR_SPI_CREATE_THREAD: return "Error creating spi thread";
    case ERR_SPI_OPEN_DEV: return "Error opening the SPI0 device (/sys/class/gpio/gpio17/value)";
    case ERR_OPEN_EDGE_DEV: return "Error opening the gpio edge device (/sys/class/gpio/gpio[io]/edge)";
    case ERR_WRITE_EDGE_DEV: return "Error writing to the gpio edge device (/sys/class/gpio/gpio[io]/edge)";
    case ERR_OPEN_I2C_NEW_DEVICE: return "Error opening the i2c new_device (/sys/class/i2c-adapter/i2c-0/new_device)";
    case ERR_WRITE_I2C_NEW_DEVICE: return "Error writing to the i2c new_device (/sys/class/i2c-adapter/i2c-0/new_device)";
    case ERR_MSG_ENQUEUE: return "Error queue.enqueue() transmit buffer full";
    case ERR_INIT_LOCK: return "Error initializing thread synchronization lock";
    default: return "Unknown error";
  }
}
