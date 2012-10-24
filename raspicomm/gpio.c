#include "gpio.h"

#define EXPORT_DEV "/sys/class/gpio/export"
// #define UNEXPORT_DEV "/sys/class/gpio/unexport"


ApiResult gpio_Export(int io)
{
	ApiResult result = SUCCESS;
	FILE* pFile;

	// open the device
	if (result == SUCCESS)
		if ((pFile = fopen(EXPORT_DEV, "w")) == NULL)
			result = ERR_OPEN_EXPORT_DEV;
    
	if (pFile != NULL)
	{
		// export the gpio by writing to the export file
		if (fprintf(pFile, "%i", io) < 0)
			result = ERR_WRITE_EXPORT_DEV;		

		// flush the changes
		fflush(pFile);

		// close the file
		fclose(pFile);
	}	

	return result;
}

ApiResult gpio_ConfigureInterrupt(int io, Edge edge)
{
	ApiResult result = SUCCESS;
	FILE* pFile;

	// Create the device name
	char file[256];
	sprintf(file, "/sys/class/gpio/gpio%i/edge", io);

	if (result == SUCCESS)
		if ((pFile = fopen(file, "w")) == NULL)
			result = ERR_OPEN_EDGE_DEV;

	if (pFile != NULL)
	{
		if (fprintf(pFile, edge == Falling ? "falling" : "raising") < 0)
			result = ERR_WRITE_EDGE_DEV;

		fflush(pFile);

		fclose(pFile);
	}

	return result;
}