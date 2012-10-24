#include "spi.h"
#include "button.h"
#include "output.h"
#include "revision.h"

#include <fcntl.h> // defines O_RDWR and O_SYNC
#include <stdlib.h> // defines malloc
#include <stdio.h>
#include <sys/mman.h> // defines PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FIXED

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define CLOCK_BASE               (BCM2708_PERI_BASE + 0x101000) /* Clocks */
#define SPI0_BASE                (BCM2708_PERI_BASE + 0x204000) /* SPI0 controller */
#define UART0_BASE               (BCM2708_PERI_BASE + 0x201000) /* Uart 0 */
#define UART1_BASE               (BCM2708_PERI_BASE + 0x215000) /* Uart 1 (not used) */


typedef struct {
  char* mem;
  char* map;
  volatile unsigned int* P;
} MemoryMap;


// the struct that holds our internals
typedef struct {
  
  int IsInitialized;
  int mem_fd;
  int Revision;

  MemoryMap gpio;
  MemoryMap spi0;
  MemoryMap clk;
  MemoryMap uart;

} Internals;

// our internals start out not initialized
static Internals _Internals = { 0, 0 };

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(_Internals.gpio.P+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(_Internals.gpio.P+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(_Internals.gpio.P+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SETALL *(_Internals.gpio.P+7)  // sets bits which are 1 ignores bits which are 0
#define GPIO_SET(o) (GPIO_SETALL = 1 << (o)) // sets the specified output
#define GPIO_CLRALL *(_Internals.gpio.P+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_CLR(o) (GPIO_CLRALL = 1 << (o)) // clears the specified output

#define GPIO_GETALL *(_Internals.gpio.P+13) // gets all inputs, 1 are active 0 are inactive
#define GPIO_GET(i) (GPIO_GETALL >> (i) & 1) // gets the specified input, 1 is active 0 is inactive

// Output2 / Red LED
int GPIO_LED2;

// defines the function of the gpios
typedef enum {

  // Buttons
  ButtonPressed = 22,
  ButtonDown = 4,
  ButtonLeft = 23,
  ButtonUp = 24,
  ButtonRight = 25,

  // Output1 / Green LED 
  GPIO_LED1 = 18 

} GPIO;

