/// \file

#ifdef __cplusplus
extern "C" {
#endif

/// Declares the ApiError enum.
/// Every api function returns an  ApiStatus code that specifies the success of the operation
typedef enum 
{
  /// Success - No Error
  SUCCESS,

  /// Already Initialized - raspicomm_Init() was already called before.
  ERR_ALREADY_INITIALIZED,

  /// opening /dev/mem did not succeed. Do you have sufficient privileges?
  ERR_OPEN_DEV_MEM,

  /// memory allocation error
  ERR_ALLOC,

  /// calling mmap() resulted in an error
  ERR_MMAP,

  /// raspicomm was not initialized. call raspicomm_Init() before calling this function.
  ERR_NOT_INITIALIZED,

  /// this method is not implemented yet. Please check back later.
  ERR_NOT_IMPLEMENTED,

  /// the function was called with an invalid parameter.
  ERR_INVALID_PARAM,

  /// Error opening the gpio export device (/sys/class/gpio/export)
  ERR_OPEN_EXPORT_DEV,

  /// Error writing to the gpio export device (/sys/class/gpio/export)
  ERR_WRITE_EXPORT_DEV,

  /// Error creating SPI Thread
  ERR_SPI_CREATE_THREAD,

  /// Error opening the SPI0 device (/sys/class/gpio17/value)
  ERR_SPI_OPEN_DEV,

  /// Error opening the gpio edge device (/sys/class/gpio/gpio[io]/edge)
  ERR_OPEN_EDGE_DEV,

  /// Error writing to the gpio edge device (/sys/class/gpio/gpio[io]/edge)
  ERR_WRITE_EDGE_DEV,

  /// Error opening the i2c new_device (/sys/class/i2c-adapter/i2c-0/new_device)
  ERR_OPEN_I2C_NEW_DEVICE,

  /// Error writing to the i2c new_device (/sys/class/i2c-adapter/i2c-0/new_device)
  ERR_WRITE_I2C_NEW_DEVICE,

  /// Error queue.enqueue() transmit buffer full
  ERR_MSG_ENQUEUE,

  /// Error initializing thread synchronization lock
  ERR_INIT_LOCK
  
} ApiResult;

#ifdef __cplusplus
}
#endif