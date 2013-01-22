/// \file
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <syslog.h>

#include <sys/socket.h>
#include <linux/in.h>

#include <poll.h>
#include <stropts.h>

// include raspicomm api
#include "../raspicomm/api.h"

#define LOG

/// <summary>Internal struct that holds the daemons settings</summary>
typedef struct {

  /// <summary>The port that the daemon will listen on.</summary>
  int port;

  /// <summary>The path of the file where the daemon will write it's pid to</summary>
  char pidfile[256];

  /// <summary>Whetever logging is enabled (=1) or disable (=0)</summary>
  int log;

} Settings;

/// <summary>Forward declaration of internal methods.</summary>
ApiResult daemon_init_raspicomm(const Settings* settings);

/// <summary>raspicommd main method</summary>
void do_work(void);

int init_pipe(void);

void init_syslog(void);

void enter_main_loop(const Settings* settings);

void daemon_shutdown(void);

int load_settings(Settings* settings);

void set_default_settings(Settings* settings);

void sr_receive(char c);

void daemon_log(const char* msg, ApiResult result);

int write_pidfile(const char* file);
