#include <stdio.h>
#include "apiresult.h"

// Exports the specified gpio
ApiResult gpio_Export(int io);

typedef enum {
	Falling = 1 << 0,
	Rising  = 1 << 1

} Edge;

// Configures the specified gpio
ApiResult gpio_ConfigureInterrupt(int io, Edge edge);