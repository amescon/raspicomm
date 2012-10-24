#include "max3140.h"

char max3140_GetBaudrateIndex(Baudrate baudrate)
{
	switch (baudrate)
	{
		case 600: return 0x7;
		case 1200: return 0xE;
		case 2400: return 0xD;
		case 4800: return 0xC;
		case 9600: return 0xB;
		case 19200: return 0xA;
		case 38400: return 0x9;
		case 57600: return 0x2;
		case 115200: return 0x1;
		case 230400: return 0x0;
		
		// default to 9600
		// default: return max3140_GetBaudrateIndex(9600); 
	}
}

unsigned int max3140_GetUARTConfiguration(Baudrate baudrate, Databits databits, Stopbits stopbits, Parity parity)
{	
	unsigned int value = 0;

	// set bits 15 and 14 for write configuration
	// value |= 1 << 15;
	// value |= 1 << 14;
	value |= (UART_R | UART_T);

	// set bit 10 for FIFO interrupt enable
	value |= UART_RM;

	// add the baudrate
	value |= max3140_GetBaudrateIndex(baudrate);

	// add the stopbits
	value |= stopbits << 6;

	// add the parity	
	value |= parity << 5;	

	// add the databits
	value |= databits << 4;
	
	return value;
}