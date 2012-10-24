#include "spi.h"

ApiResult raspicomm_InitRs485(Baudrate baudrate, Databits databits, Stopbits stopbits, Parity parity, void (*recv_callback) (char))
{
	ApiResult result = SUCCESS;

	// update the settings according to the baudrate
	_SPISettings.swBackSleep = 10000000 / baudrate;

	// store the settings for later use
	_SPISettings.baudrate = baudrate;
	_SPISettings.databits = databits;
	_SPISettings.stopbits = stopbits;
	_SPISettings.parity   = parity;

	// _SPISettings.config   = baudrateIndex + 0xc400;
	_SPISettings.config = max3140_GetUARTConfiguration(_SPISettings.baudrate, _SPISettings.databits, _SPISettings.stopbits, _SPISettings.parity);

  _SPISettings.recv_callback = recv_callback;

	return result;
}

void spi_HandleSpiReceive()
{
	int rxdata;
	int txdata;

	// issue a read data command to discover the cause of the interrupt
	rxdata = spi_Send(0);
	// printf("rxdata: %x", rxdata);
	
	while (rxdata & UART_R) //rx bit is set
	{
		enqueue(&_SPISettings.rxqueue, rxdata & 0x00FF);

		//read subsequent bytes
		rxdata = spi_Send(0);
	}	

	if ((rxdata & UART_T) && (_SPISettings.config & UART_TM))
	{

    // acquire Queue Lock
    if (pthread_rwlock_wrlock(&_SPISettings.QueueLock) == 0)
    {

		  //transmit data
		  if (!isQueueEmpty(&_SPISettings.txqueue))
		  {
			  txdata = dequeue(&_SPISettings.txqueue);
			  spi_Send(0x8000 | txdata);		
		  }		
		  else
		  {
			  //mask transmit butter empty interrupt
			  _SPISettings.config = _SPISettings.config & ~ 0x0800; //clear TM bit
  	    _SPISettings.config = _SPISettings.config | 0xc000; //set bits 15 and 14
			  spi_Send(_SPISettings.config);
		
			  usleep(_SPISettings.swBackSleep);			
			  //enable receive by disabling RTS (TE set so that no data is sent)
			  spi_Send(0x8600);
		  }

      // release Queue Lock
      pthread_rwlock_unlock(&_SPISettings.QueueLock);
	
    }
	}
	
}

int spi_Send(int mosi)
{
	unsigned char v1,v2,rec_c;
  int status,w;
  // Set up for single ended, MS comes out first
  v1 = mosi >> 8;
  v2 = mosi & 0x00FF;

  // Enable SPI interface: Use CS 0 and set activate bit
  SPI0_CNTLSTAT = SPI0_CS_CHIPSEL0|SPI0_CS_ACTIVATE;

	// Delay to make sure chip select is high for a short while
  // usleep(100);

  // Write the command into the FIFO 
  SPI0_FIFO = v1;
  SPI0_FIFO = v2;

  // wait for SPI to be ready
  // This will take about 16 micro seconds
  do {
     status = SPI0_CNTLSTAT;
  } while ( (status & SPI0_CS_DONE) == 0 );
  SPI0_CNTLSTAT = SPI0_CS_DONE; // clear the done bit

  // Data from the ADC chip should now be in the receiver
  // read the received data
  v1 = SPI0_FIFO;
  v2 = SPI0_FIFO;

  // printf("SPI:%x - ret=%x\n", mosi, (v1<<8) | (v2));
 
  return ( (v1<<8) | (v2) );
}


void spi_InitMax3140(void)
{	
	spi_Send( _SPISettings.config);
	spi_Send( 0x8600 ); // enable receive by disabling RTS (TE set so that no data is sent)
}

typedef struct {

		int file;

} ThreadParam;

// polling thread that blocks as long as no data is received
void *spi_PollingThread(void* args)
{
	// We're getting the file handle from the arguments	
	ThreadParam* pParam = (ThreadParam*)args;	

	int i;
	const int fdCount = 1;

	// create the polling parameter
	struct pollfd fds[fdCount];
	fds[0].fd = (int)pParam->file;
	fds[0].events = POLLPRI;

	// setup the read buffer
	char buffer[BUFFER_LENGTH];

	int retValue = 0;
	int bytesRead;

	// clear the gpio stream
	bytesRead = read(pParam->file, buffer, BUFFER_LENGTH-1);

	// read data from it in a loop
	while (_SPISettings.ShutdownPollingThread == 0)
	{
		lseek(pParam->file, 0, SEEK_SET); // reset the file to the start of the file

		retValue = poll(fds, fdCount, 500); // use a timeout so that we can check for shutdown

		switch (retValue)
		{
			case -1: break; // failure, check errno for details
			case 0:  break; // timeout				

			default:
				// check what happened
				for (i = 0; i < fdCount; i++)
				{
					if (fds[i].revents & POLLPRI) // high-prority data may be read without blocking
					{
						bytesRead = read(pParam->file, buffer, BUFFER_LENGTH-1);

						// call the callback function when data arrives
						spi_HandleSpiReceive();

            void (*callback)(char);
            callback = _SPISettings.recv_callback;

            while(!isQueueEmpty(&_SPISettings.rxqueue))
            {
              char c = dequeue(&_SPISettings.rxqueue);
              if (callback != NULL)
                callback(c);
            }
					}
				}
				break;
		} 
	}

	close(pParam->file);
	free(pParam);
}

// this function starts the polling thread
ApiResult spi_StartThread(void)
{
	_SPISettings.ShutdownPollingThread = 0;	

	ThreadParam* pParam = (ThreadParam*)malloc(sizeof(ThreadParam));

	ApiResult result = SUCCESS;

	// Open the device file for the polling thread
	if (result == SUCCESS)		
		if ( (pParam->file = open(SPI0_DEV, O_RDONLY)) == -1)
			result = ERR_SPI_OPEN_DEV;

	int value;
	if (result == SUCCESS)
	  if ( (value = pthread_create(&_SPISettings.PollingThread, NULL, spi_PollingThread, pParam)) != 0 )
	    result = ERR_SPI_CREATE_THREAD;

	return result;
}

ApiResult init_queuelock(pthread_rwlock_t* lock)
{
  ApiResult error = SUCCESS;

  // init the lock
  if ( pthread_rwlock_init(lock, NULL) != 0 )
    error = ERR_INIT_LOCK;

  return error;
}


ApiResult raspicomm_init_spi(volatile unsigned int* spi0)
{
  // Store the spi0 pointer so that we can use the macros that depend on it
  _SPISettings.spi0 = spi0;

  // printf("spi0: %i\n", spi0);

  ApiResult result = SUCCESS;

  const int io = 17;

  // export gpio 17 into userspace using sysfs
  if (result == SUCCESS)
  	result = gpio_Export(io);

  if (result == SUCCESS)
  	result = gpio_ConfigureInterrupt(io, Falling);
  
  if (result == SUCCESS)
  {
  	// Configure the SPI0 port

  	// Want to have 1 MHz SPI clock.
  	// Assume 250 Mhz system clock
  	// So divide 250MHz system clock by 250 to get 1MHz45
  	SPI0_CLKSPEED = 250;

  	// clear FIFOs and all status bits
  	SPI0_CNTLSTAT = SPI0_CS_CLRALL;
  	SPI0_CNTLSTAT = SPI0_CS_DONE; // make sure done bit is cleared	  
  }

  if (result == SUCCESS)
    spi_InitMax3140();

  // initialize the QueueLock
  if (result == SUCCESS)
    result = init_queuelock(&_SPISettings.QueueLock);

  // Start the polling thread
  if (result == SUCCESS)
   	result = spi_StartThread();  

  return result;
  
}

ApiResult raspicomm_shutdown_spi()
{
	ApiResult result = SUCCESS;

	// Signal the polling thread that we want to shutdown
	_SPISettings.ShutdownPollingThread = 1;

	// Wait for the polling thread to shutdown
	pthread_join(_SPISettings.PollingThread, NULL);

	return result;
}

ApiResult raspicomm_spi_write(char *str, int length, int* sentBytes)
{
	ApiResult result = raspicomm_IsInitialized();

	if (length == 0)
		result == ERR_INVALID_PARAM;
	else
	{
		int receive, i;

    // acquire Queue Lock
    if (pthread_rwlock_wrlock(&_SPISettings.QueueLock) == 0)
    {

		  for(i=0; i<length; i++)
		  {
			  if (enqueue(&_SPISettings.txqueue, str[i]) < 0)
        {
          break;
        }          
		  }

      // set the sent bytes
      if (sentBytes != NULL)
        *sentBytes = i;

      // check if all bytes could be enqueued
      if (i != length)
        result = ERR_MSG_ENQUEUE;

		  //enable transmit buffer empty interrupt

		  _SPISettings.config = _SPISettings.config | 0xC800; //set bit 15, 14, TM bit
		  receive = spi_Send(_SPISettings.config);

		  if (receive & UART_T)  //transmit buffer is ready to accept data
			  spi_Send ( 0x8000 | dequeue(&_SPISettings.txqueue) );

      // release Queue Lock
      pthread_rwlock_unlock(&_SPISettings.QueueLock);
      
    }
	}

	return result;
}

