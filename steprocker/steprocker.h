/// \file
/// <summary>
/// The steprocker api implements the tmcl protocoll (=trinamic motion control language). For more information check http://tmctechlibcd/Software/TMCL/TMCL_reference.pdf
/// </summary>

#include <stdio.h>
#include <string.h>
#include "../raspicomm/api.h"
#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif

/// struct of tmcl command that gets send to the steprocker 
struct tmclcmd
{
	unsigned char moduleAddress;
	unsigned char command;
	unsigned char type;
	unsigned char motor;
	long value;
	unsigned char checksum;
};

/// forward declarations of internal functions 
unsigned char steprocker_CalcChecksum(char *cmd);
void steprocker_UpdateCommand(struct tmclcmd *cmd, unsigned char moduleAddress, unsigned char command, unsigned char type, unsigned char motor, long value);

#define RECEIVEBUFFER_LENGTH 9

/// struct that holds internal fields 
struct internals {
  
  // the current command
  struct tmclcmd cmd; 
  
  int receiveBufferIndex;
  char receiveBuffer[RECEIVEBUFFER_LENGTH];
};

/// steprocker lib internals 
static struct internals _Internals;

/// available steprocker commands 
typedef enum {	

	// motion commands
	SR_CMD_ROR = 1,
	SR_CMD_ROL = 2,
	SR_CMD_MST = 3,
	SR_CMD_MVP = 4,
	SR_CMD_RFS = 13,
	SR_CMD_SCO = 30,
	SR_CMD_GCO = 31,
	SR_CMD_CCO = 32,

	// parameter commands
	SR_CMD_SAP  = 5,
	SR_CMD_GAP  = 6,
	SR_CMD_STAP = 7,
	SR_CMD_RSAP = 8,
	SR_CMD_SGP  = 9,
	SR_CMD_GGP  = 10,
	SR_CMD_STGP = 11,
	SR_CMD_RSGP = 12,

	// I/O port commands
	SR_CMD_SIO = 14,
	SR_CMD_GIO = 15,
	SR_CMD_SAC = 29,

	// control commands
	SR_CMD_JA   = 22,
	SR_CMD_JC   = 21,
	SR_CMD_COMP = 20,
	SR_CMD_CLE  = 36,
	SR_CMD_CSUB = 23,
	SR_CMD_RSUB = 24,
	SR_CMD_WAIT = 27,
	SR_CMD_STOP = 28,

	// calculation commands
	SR_CMD_CALC  = 19,
	SR_CMD_CALCX = 33,
	SR_CMD_AAP   = 34,
	SR_CMD_AGP   = 35

} StepRockerCommands;

#ifdef __cplusplus
}
#endif