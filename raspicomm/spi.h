#include <pthread.h> // defines pthread_rwlock_t
#include <poll.h>
#include <stdlib.h>
#include "gpio.h"
#include "max3140.h"
#include "queue.h"
#include <fcntl.h>
#include <string.h>

#define SPI0_DEV "/sys/class/gpio/gpio17/value"

#ifdef __cplusplus
extern "C" {
#endif


// SPI specific settings
typedef struct
{
  volatile unsigned int* spi0;
  pthread_t PollingThread;
  int ShutdownPollingThread;

  // int baudrateIndex;
  int config;
  int swBackSleep;
  Baudrate baudrate;
  Databits databits;
  Stopbits stopbits;
  Parity parity;

  // receive callback
  void (*recv_callback)(char);

  struct queue txqueue;
  struct queue rxqueue;

  pthread_rwlock_t QueueLock;


} SPISettings;

// Store the settings in a global variable
static SPISettings _SPISettings;

#define BUFFER_LENGTH 256

// SPI macros
#define SPI0_CNTLSTAT *(_SPISettings.spi0 + 0)
#define SPI0_FIFO     *(_SPISettings.spi0 + 1)
#define SPI0_CLKSPEED *(_SPISettings.spi0 + 2)

// SPI0_CNTLSTAT register bits

#define SPI0_CS_CS2ACTHIGH   0x00800000 // CS2 active high
#define SPI0_CS_CS1ACTHIGH   0x00400000 // CS1 active high
#define SPI0_CS_CS0ACTHIGH   0x00200000 // CS0 active high
#define SPI0_CS_RXFIFOFULL   0x00100000 // Receive FIFO full
#define SPI0_CS_RXFIFO3_4    0x00080000 // Receive FIFO 3/4 full
#define SPI0_CS_TXFIFOSPCE   0x00040000 // Transmit FIFO has space
#define SPI0_CS_RXFIFODATA   0x00020000 // Receive FIFO has data
#define SPI0_CS_DONE         0x00010000 // SPI transfer done. WRT to CLR!
#define SPI0_CS_MOSI_INPUT   0x00001000 // MOSI is input, read from MOSI (BI-dir mode)
#define SPI0_CS_DEASRT_CS    0x00000800 // De-assert CS at end
#define SPI0_CS_RX_IRQ       0x00000400 // Receive irq enable
#define SPI0_CS_DONE_IRQ     0x00000200 // irq when done
#define SPI0_CS_DMA_ENABLE   0x00000100 // Run in DMA mode
#define SPI0_CS_ACTIVATE     0x00000080 // Activate: be high before starting
#define SPI0_CS_CS_POLARIT   0x00000040 // Chip selects active high
#define SPI0_CS_CLRTXFIFO    0x00000020 // Clear TX FIFO    (auto clear bit)
#define SPI0_CS_CLRRXFIFO    0x00000010 // Clear RX FIFO    (auto clear bit)
#define SPI0_CS_CLRFIFOS     0x00000030 // Clear BOTH FIFOs (auto clear bit)
#define SPI0_CS_CLK_IDLHI    0x00000008 // Clock pin is high when idle
#define SPI0_CS_CLKTRANS     0x00000004 // 0=first clock in middle of data bit
                                        // 1=first clock at begin of data bit
#define SPI0_CS_CHIPSEL0     0x00000000 // Use chip select 0
#define SPI0_CS_CHIPSEL1     0x00000001 // Use chip select 1
#define SPI0_CS_CHIPSEL2     0x00000002 // No chip select (e.g. use GPIO pin)
#define SPI0_CS_CHIPSELN     0x00000003 // No chip select (e.g. use GPIO pin)

#define SPI0_CS_CLRALL      (SPI0_CS_CLRFIFOS|SPI0_CS_DONE)

// function used to initialize the spi
ApiResult raspicomm_init_spi(volatile unsigned int* spi0);

ApiResult raspicomm_shutdown_spi(void);
ApiResult raspicomm_InitRs485(Baudrate baudrate, Databits databits, Stopbits stopbits, Parity parity, void (*rec_callback)(char));


ApiResult spi_StartThread(void);
void spi_InitMax3140(void);

#ifdef __cplusplus
}
#endif