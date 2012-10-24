typedef enum {
	   BAUDRATE_600 = 600,
	  BAUDRATE_1200 = 1200,
	  BAUDRATE_2400 = 2400,
	  BAUDRATE_4800 = 4800,
 	  BAUDRATE_9600 = 9600,
	 BAUDRATE_19200 = 19200,
	 BAUDRATE_38400 = 38400,
	 BAUDRATE_57600 = 57600,
	BAUDRATE_115200 = 115200,
	BAUDRATE_230400 = 230400
} Baudrate;

typedef enum {
	STOPBITS_ONE = 0,
	STOPBITS_TWO = 1
} Stopbits;

typedef enum {
	DATABITS_7 = 1,
	DATABITS_8 = 0
} Databits;

typedef enum {
	PARITY_OFF = 0,
	PARITY_ON = 1
} Parity;

typedef enum {

	UART_R     = 1 << 15,
	UART_T     = 1 << 14,
	UART_FEN   = 1 << 13,
	UART_SHDNo = 1 << 12,
	UART_TM    = 1 << 11,
	UART_RM    = 1 << 10,
	UART_PM    = 1 << 9,
	UART_RAM   = 1 << 8,
	UART_IR    = 1 << 7,
	UART_ST    = 1 << 6,
	UART_PE    = 1 << 5,
	UART_L     = 1 << 4,
	UART_B3    = 1 << 3,
	UART_B2    = 1 << 2,
	UART_B1    = 1 << 1,
	UART_B0    = 1 << 0

} UartFlags;


char max3140_GetBaudrateIndex(Baudrate baudrate);

unsigned int max3140_GetUARTConfiguration(Baudrate baudrate, Databits databits, Stopbits stopbits, Parity parity);
