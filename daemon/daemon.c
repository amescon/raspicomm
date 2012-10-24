/// \file
#include "daemon.h"

/* set and used by init_syslog() */
static char name[50];

/* Stay 1 byte below the buffer size of the queue, or the circular queue will overflow */
#define RECEIVEBUFFERSIZE 255 

static int _readpipe, _writepipe;

/// <summary>Main Entry Point</summary>
/// <description>Gets the daemon up and running.</description>
int main(void) {

  /* Our process ID and Session ID */
  pid_t pid, sid;

  /* Fork off the parent process */
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then
  we can exit the parent process. */
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  /* Change the file mode mask */
  umask(0);

  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  /* Change the current working directory */
  if ((chdir("/")) < 0) {
    exit(EXIT_FAILURE);
  }

  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  /* the daemon specific work starts here */
  do_work();

  /* daemon process is exiting gracefully */
  exit(EXIT_SUCCESS);
}

/// <summary>Our raspicomm daemon specific work gets done.</summary>
void do_work(void)
{
  /* setup the syslog for logging */
  init_syslog();

  /* read the settings from the settings file */
  Settings settings; int ret;
  if ((ret = load_settings(&settings)) < 0)
  {
    //syslog(LOG_ERR, "loading settings failed (%i)", ret); 
    exit(EXIT_FAILURE);
  }
    
  if (settings.log)
  {
    syslog(LOG_INFO, "starting...");

    syslog(LOG_INFO, "using %s: port '%i' and pidfile '%s' and log=%i", 
      ret == 0 ? "default configuration" : "configurationfile",
      settings.port, 
      settings.pidfile,
      settings.log);
  }  

  /* write the pidfile */
  if ( write_pidfile(settings.pidfile) != 0)
    if (settings.log)
      syslog(LOG_ERR, "writing PIDFILE '%s' failed", settings.pidfile);

  /* use the settings to initialize the raspicomm */
  if (daemon_init_raspicomm(&settings) != SUCCESS)
    exit(EXIT_FAILURE);

  /* init the pipe */
  if (init_pipe() != 0)
    if (settings.log)
      syslog(LOG_ERR, "init_pipe failed");

  /* block and work */
  enter_main_loop(&settings);
  
  /* tear down raspicomm */
  daemon_shutdown();

}

/// <summary>Writes the pid of the current process to the specified file.</summary>
int write_pidfile(const char* fileName)
{
  FILE* file;
  if ( (file = fopen(fileName, "w")) == NULL )
    return -1;
  else
  {
    pid_t pid = getpid();

    char buffer[256];
    sprintf(buffer, "%i", pid);
    fwrite(buffer, 1, strlen(buffer), file);

    fclose(file);
    return 0;
  }
}

/// <summary>Initializes the pipe.</summary>
/// <returns>Returns 0 on success, otherwise -1</returns>
int init_pipe(void)
{
  int fd[2];
  if ( pipe(fd) == -1 )
  {
    //syslog(LOG_ERR, "error initializing pipe\n");
    return -1;
  }
  else
  {
    _readpipe = fd[0];
    _writepipe = fd[1];
    return 0;
  }
}

/// <summary>Initializes the syslog.</summary>
void init_syslog(void)
{
  /* set the name variable with our daemons name + pid */
  pid_t pid = getpid();    
  sprintf(name, "raspicommd[%i]", pid);

  // initialize the syslog
  openlog(name, 0, LOG_DAEMON);  
}

/// <summary>Shuts the daemon down.</summary>
void daemon_shutdown(void)
{
  /* the last log message */
  syslog(LOG_INFO, "shutting down...\n");

  /* close the syslog */
  closelog(); 

  /* shutdown the raspicomm */
  raspicomm_Shutdown();
}

/// <summary>Initializes the raspicomm.</summary>
ApiResult daemon_init_raspicomm(const Settings* settings) 
{
  /* initialize the raspicomm */
  ApiResult result = raspicomm_Init(REVISION_LATEST);

  if (result != SUCCESS && settings->log)
    syslog(LOG_ERR, "initializing raspicomm failed(%i): %s\n", result, raspicomm_TranslateResult(result)); 

  /* configure the Rs485 port for steprocker usage */
  result = raspicomm_InitRs485(BAUDRATE_9600, DATABITS_8, STOPBITS_ONE, PARITY_OFF, sr_receive);

  if (result != SUCCESS && settings->log)
    syslog(LOG_ERR, "configuring Rs485 for steprocker usage failed(%i): %s\n", result, raspicomm_TranslateResult(result));

  return result;
}

/// <summary>Fills the supplied settings struct with default values.</summary>
void set_default_settings(Settings* settings)
{
  if (settings != NULL)
  {
    // set the default port    
    settings->port = 3333;
    
    // set the default pidfile name
    strcpy(settings->pidfile, "/var/run/raspicommd.pid");

    // log events per default
    settings->log = 1;
  }
}

/// <summary>Fill the supplied settings struct by reading them from a configuration file</summary> 
/// <returns>Returns -1 when settings == NULL
/// returns 0 when configuration file could not be read and default values were used
/// returns 1 when configuration file was successfully read.</returns>
int load_settings(Settings* settings)
{
  if (settings == NULL)
    return -1;

  // set the default values for the settings
  set_default_settings(settings);

  // read the configuration settings from a file instead 
  FILE* file;
  if ( (file = fopen("/etc/raspicommd.conf", "r")) == NULL)
    return 0;
  else
  {
    // read the configuration file line by line
    char buffer[256];
    while ( fgets(buffer, 256, file) != NULL )
    {
      //syslog(LOG_INFO, "read config: '%s'", read);      
      if ( strncmp("PORT=", buffer, 5) == 0)
      {
        char portBuffer[255]; memset(portBuffer, 0, 255);
        strncpy(portBuffer, buffer + 5, strlen(buffer) - 6);        
        settings->port = atoi(portBuffer);
      }
      else if (strncmp("PIDFILE=", buffer, 8) == 0)
      {
        memset(settings->pidfile, 0, 255);
        strncpy(settings->pidfile, buffer + 8, strlen(buffer) - 9);
      }
      else if (strncmp("LOG=", buffer, 4) == 0)
      {
        char logBuffer[2]; memset(logBuffer, 0, 2);
        strncpy(logBuffer, buffer + 4, strlen(buffer) - 4);
        settings->log = atoi(logBuffer);
      }
    }

    // close the configuration file
    fclose(file);
  }
  

  return 1;
}

/// <summary>Enters the main loop of the daemon.</summary>
void enter_main_loop(const Settings* pSettings)
{
  /* receive buffer */
  char buffer[RECEIVEBUFFERSIZE];

  /* The main Loop */
  while (1) 
  {
    // open up a socket and wait for a user to connect
    sleep(1);

    /* socket() */
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      //syslog(LOG_ERR, "error socket(): %i\n", sockfd);
      continue;
    }
    //syslog(LOG_INFO, "socket()\n");

    // todo refactor the code so that the server socket gets close before we open a new one

    /* bind() */
    struct sockaddr_in address; memset((void*)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(pSettings->port);
    address.sin_addr.s_addr = INADDR_ANY;

    if ( bind(sockfd, (struct sockaddr*) &address, sizeof(address)) == -1 )
    {
      //syslog(LOG_ERR, "error bind(), errno=%i\n", errno);
      continue;
    }
    //syslog(LOG_INFO, "bind()\n");

    /* listen() */
    if ( listen(sockfd, 0) == -1 )
    {
      //syslog(LOG_ERR, "error listen(), errno=%i\n", errno);
      continue;
    }
    //syslog(LOG_INFO, "listen()\n");

    /* accept clients loop */
    while (1)
    {
      sleep(1);

      //syslog(LOG_INFO, "awaiting connection\n");

      /* accept() */
      struct sockaddr_in clientAddress;    
      int addrlen = sizeof(clientAddress);
      int client_sockfd;
      if ( (client_sockfd = accept(sockfd, (struct sockaddr*) &clientAddress, &addrlen)) == -1 )
      {
        //syslog(LOG_ERR, "error accept(), errno=%i\n", errno);
        continue;
      }
      //syslog(LOG_INFO, "client (%s) accepted\n", inet_ntoa(clientAddress.sin_addr));


      // receive loop           
      short pollevent = POLLIN;
      const int fdCount = 2;
      struct pollfd fds[fdCount];
	    fds[0].fd = client_sockfd;
	    fds[0].events = pollevent;
      fds[1].fd = _readpipe;
      fds[1].events = pollevent;
           
      int stopPolling = 0;
      do
      {
        // wait for a pipe signal or a socket read      
        int numReady = poll(fds, fdCount, -1);

        if (numReady == -1 || 
            numReady == 0)
          stopPolling = 1;
          
        if (numReady > 0)
        {
          /* if socket is ready to read */          
          if (fds[0].revents & pollevent)            
          {
            /* recv */
            int count = recv(client_sockfd, buffer, RECEIVEBUFFERSIZE, 0);

            /* forward the received bytes to the raspicomm */
            if (count > 0)
            {
              int sentBytes = 0;

              ApiResult result = raspicomm_spi_write(buffer, count, &sentBytes);

              //// log errors
              //if (result != SUCCESS) {               
              //  daemon_log("raspicomm_spi_write", result);
              //  //exit(-1);
              //}              

            }
            else
              stopPolling = 1;              
          }

          /* if the pipe is ready to read */
          if (fds[1].revents & pollevent)            
          {
            /* read from pipe */
            int count = read(_readpipe, buffer, RECEIVEBUFFERSIZE);

            /* forward the bytes to the connected client */
            if (count > 0)
              send(client_sockfd, buffer, count, 0);
          
          }
        }
      } while (!stopPolling);


      //syslog(LOG_INFO, "client (%s) disconnected\n", inet_ntoa(clientAddress.sin_addr));

      /* close the client socket */
      close(client_sockfd);
    }    

  }
}

/// <summary>Receive callback that get called when a byte is received from the socket.</summary>
void sr_receive(char c)
{
  /* send the received byte from the raspicomm into the pipe that the socket thread is listening to */
  write(_writepipe, &c, 1);
}

/// <summary>Helper function that is use to log an ApiResult.</summary>
void daemon_log(const char* msg, ApiResult result)
{
  syslog(LOG_ERR, "%s: ApiResult (%i): '%s'", msg, result, raspicomm_TranslateResult(result)); 
}
