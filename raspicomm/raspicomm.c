#include "raspicomm.h"


ApiResult raspicomm_IsInitialized(void)
{
  return _Internals.IsInitialized == 1 ? SUCCESS : ERR_NOT_INITIALIZED;
}

ApiResult raspicomm_GetRaspberryPiRevision(Revision* revision)
{
  ApiResult result = raspicomm_IsInitialized();

  if (result == SUCCESS && revision == NULL)
      result = ERR_INVALID_PARAM;

  if (result == SUCCESS)
    *revision = _Internals.Revision;

  return result;
}

// Helper function of raspicomm_Init() - Sets the mem_fd pointer if successful
ApiResult init_mem(Internals* i)
{
  if ((i->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
    return ERR_OPEN_DEV_MEM;   
  else
    return SUCCESS;
}

// Helper function of raspicomm_Init()
ApiResult init_memorymap(int mem_fd, MemoryMap* mm, int offset)
{
  ApiResult
 error = SUCCESS;

  // Allocate the block
  if ((mm->mem = (char*)malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
    error = ERR_ALLOC;

  if (error == SUCCESS)
  {
    // Make sure the pointeris on 4K boundary
    if ((unsigned long)mm->mem % PAGE_SIZE)
      mm->mem += PAGE_SIZE - ((unsigned long)mm->mem % PAGE_SIZE);

    // map it
    mm->map = (char*) (unsigned char*)mmap(
      (caddr_t)mm->mem,
      BLOCK_SIZE,
      PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_FIXED,
      mem_fd,
      offset
      );

    if ((long)mm->map < 0)
      error = ERR_MMAP;  
  }

  // Set the pointer
  mm->P = (volatile unsigned int*)mm->map;

  return error;
}

// Helper function of raspicomm_Init() - Sets the ios up for working RasPiComm
void raspicomm_init_gpio(Revision revision)
{
  // set the revision specific gpios
  // raspberry pi revision 1.0 uses 21 
  // raspberry pi revision 2.0 uses 27

  int gpioled2;
  switch (revision)
  {
    case REVISION_1:
      gpioled2 = 21;
      break;

    case REVISION_2:   
    default:
      gpioled2 = 27;
      break;
  }
  GPIO_LED2 = gpioled2;

  INP_GPIO(0); SET_GPIO_ALT(0,0); //I2C SDA
  INP_GPIO(1); SET_GPIO_ALT(1,0); //I2C SCL
  INP_GPIO(4);
  INP_GPIO(7);  SET_GPIO_ALT(7,0);
  INP_GPIO(8);  SET_GPIO_ALT(8,0);
  INP_GPIO(9);  SET_GPIO_ALT(9,0);
  INP_GPIO(10); SET_GPIO_ALT(10,0);
  INP_GPIO(11); SET_GPIO_ALT(11,0);
  // 14 and 15 are already set to UART mode
  // by Linux. Best if we don't touch them
  // INP_GPIO(14); SET_GPIO_ALT(14,0);
  // INP_GPIO(54); SET_GPIO_ALT(15,0);
  INP_GPIO(17);
  INP_GPIO(18);  OUT_GPIO(18);

  // initialize the revision dependent gpio
  INP_GPIO( GPIO_LED2 ); OUT_GPIO( GPIO_LED2 );
  
  INP_GPIO(22);  
  INP_GPIO(23);
  INP_GPIO(24);
  INP_GPIO(25);
}

// Initializes the real time clock
ApiResult raspicomm_init_rtc(void)
{
  // ds1307 0x68 /sys/class/i2c-adapter/i2c-0/new_device
  ApiResult result = SUCCESS;
  FILE* pFile;

  const char msg[] = "ds1307 0x68";

  if (result == SUCCESS)
   if ( (pFile = fopen("/sys/class/i2c-adapter/i2c-0/new_device", "w")) == NULL )
    result = ERR_OPEN_I2C_NEW_DEVICE;
  
  if (pFile != NULL)
  {
    // result = ERR_NOT_IMPLEMENTED;
    // fwrite()
    if (fprintf(pFile, msg) < 0)
      result = ERR_WRITE_I2C_NEW_DEVICE;

    fflush(pFile);

    fclose(pFile);
  }

  return result;
}

// Set the internal revision
void raspicomm_init_revision(Revision revision)
{
  // default to the latest revision
  int rev = REVISION_LATEST;

  if (revision != REVISION_DEFAULT)
    rev = revision;

  // if the default revision is specified, try to read it from the configuration file
  else
  {
    // read the configuration file
    FILE* file;

    if ( (file = fopen("/etc/raspicomm.conf", "r")) != NULL)
    {
      // read the config file line by line
      char buffer[256];
      while (fgets(buffer, 256, file) != NULL)
      {
        if (strncmp("REV=", buffer, 4) == 0)
        {
          char revBuffer[255]; memset(revBuffer, 0, 255);
          strncpy(revBuffer, buffer + 4, strlen(buffer) - 5);
          rev = atoi(revBuffer);
        }
      }

      fclose(file);
    }
  }

  _Internals.Revision = rev;
}

ApiResult raspicomm_Init(Revision revision)
{
  ApiResult result;
  result = SUCCESS;  

  // get a pointer to our internals
  Internals* i = &_Internals;

  // check if we're already initialized
  if (i->IsInitialized)
  {
    result = ERR_ALREADY_INITIALIZED;
  }

  // store the supplied revision
  if (result == SUCCESS)    
    raspicomm_init_revision(revision);

  // real time clock initialization is now done by boot upscript /etc/init.d/rtc.sh
  /*if (result == SUCCESS)
    result = raspicomm_init_rtc();*/

  // open /dev/mem
  if (result == SUCCESS)
    result = init_mem(i);

  // Setup GPIO
  if (result == SUCCESS)
    result = init_memorymap(i->mem_fd, &i->gpio, GPIO_BASE);

  // Setup clock
  if (result == SUCCESS)
    result = init_memorymap(i->mem_fd, &i->clk, CLOCK_BASE);

  // Setup SPI0
  if (result == SUCCESS)
    result = init_memorymap(i->mem_fd, &i->spi0, SPI0_BASE);

  // Setup UART
  if (result == SUCCESS)
    result = init_memorymap(i->mem_fd, &i->uart, UART0_BASE);

  // initialize the gpios
  if (result == SUCCESS)
    raspicomm_init_gpio(i->Revision);

  // if (result == SUCCESS)
  //   result = raspicomm_InitRs485(BAUDRATE_9600, DATABITS_8, STOPBITS_ONE, PARITY_OFF);

  // initialize the spi
  if (result == SUCCESS)
    result = raspicomm_init_spi(i->spi0.P);

  // We're initialized
  if (result == SUCCESS)
    i->IsInitialized = 1;

  return result;
}

ApiResult raspicomm_GetButtons(Buttons* buttons)
{  
  ApiResult result = raspicomm_IsInitialized();  

  if (result == SUCCESS)
  {   
    // Set the buttons    
    (*buttons) = GPIO_GET(ButtonLeft)    << 0 |                
                 GPIO_GET(ButtonUp)      << 1 |
                 GPIO_GET(ButtonRight)   << 2 |
                 GPIO_GET(ButtonDown)    << 3 |
                 GPIO_GET(ButtonPressed) << 4;
  }

  return result;
}

ApiResult raspicomm_SetOutputs(Outputs outputs)
{
  ApiResult result = raspicomm_IsInitialized();
    
  if (result == SUCCESS)
  {
    if ( LED1 == (outputs & LED1) )
      GPIO_SET(GPIO_LED1);

    if ( LED2 == (outputs & LED2) )
      GPIO_SET(GPIO_LED2);        
  }

  return result;
}

ApiResult raspicomm_ClearOutputs(Outputs outputs)
{
  ApiResult result = raspicomm_IsInitialized();

  if (result == SUCCESS)
  {
    if ( LED1 == (outputs & LED1) )
      GPIO_CLR(GPIO_LED1);

    if ( LED2 == (outputs & LED2) )
      GPIO_CLR(GPIO_LED2);
  }

  return result;
}

ApiResult raspicomm_GetOutputs(Outputs* outputs)
{
  ApiResult result = raspicomm_IsInitialized();

  if (result == SUCCESS)
  {    
    (*outputs) = GPIO_GET(GPIO_LED1) | 
                 GPIO_GET(GPIO_LED2) << 1;
  }

  return result;
}

ApiResult raspicomm_Shutdown(void)
{
  ApiResult result = raspicomm_IsInitialized();

  if (result == SUCCESS)  
    result = raspicomm_shutdown_spi();

  return result;
}

ApiResult raspicomm_GetTime(time_t time)
{
  ApiResult result = raspicomm_IsInitialized();

  if (result == SUCCESS)
  {
    // TODO
    result = ERR_NOT_IMPLEMENTED;
  }

  return result;
}

ApiResult raspicomm_SetTime(time_t time)
{
  ApiResult result = raspicomm_IsInitialized();

  if (result == SUCCESS)
  {
    // TODO
    result = ERR_NOT_IMPLEMENTED;
  }

  return result;
}