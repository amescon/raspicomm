/// \file
#include "demo.h"

/// <summary>Prints the help to the standard output</summary>
void print_help(void)
{
  printf("raspicomm demo application\n");
  printf("\n");
  printf("usage: raspicommdemo --revision=[1|2] --[option]\n");
  printf("\n");
  printf("required:\n");
  printf("    required:      revision number of the raspberry pi. 1 or 2.\n");
  printf("options:\n");
  printf("  General\n");
  printf("    interactive:   enters interactive mode\n");
  printf("    [none]:\n");
  printf("    help:          this summary\n");
  printf("  I/O\n");
  printf("    getoutputs:    prints the state of the outputs\n");
  printf("    setoutputs:    sets the outputs, which also turns leds on\n");
  printf("    unsetoutputs:  unsets the outputs, which also turns the off\n");
  printf("    getbuttons:    prints the input state of the buttons\n");
  printf("    leddemo:       shows a quick led demo\n");
  printf("\n");
  printf("  LCD:\n");
  printf("    lcddemo:       shows hello world on the lcd display\n");
  printf("    display:       writes keyboard input directly to lcd display\n");
  printf("\n");
  printf("example usage: raspicommdemo --revision=2 --setoutputs\n");
  printf("\n");
}

/// <summary>Enters the display type mode where every typed keyboard strole is shown on the display</summary>
void enter_display_type_mode()
{  
  char c;
  char x = 0, y = 0;

  display_ClearScreen();
  display_SetXY(0, 0);

  printf("TYPE MODE: ESC to cancel, ENTER for newline\n");

  while (1)
  {
    c = terminal_getchar();

    switch (c)
    {
    case 27:      
      return;    

    case 10:
      y++;
      y %= MAXROWS;
      x = 0;
      display_SetXY(x, y);
      break;

    default:
      display_PrintChar(c);
      x++;
      x %= MAXCOLUMNS;
      break;
    }
  }
}

/// <summary>Enters the general display mode</summary>
void enter_display_mode()
{
  DisplayResult result = 0;
  char c;
  int inverse = 0;

  while (1)
  {
    printf("DISPLAY MODE: (q)uit, (-) display off, (+) display on, (i)nverse toggle, (t)ype, (c)lear screen\n");

    c = terminal_getchar();

    switch (c)
    {
      case 27:
      case 'q': return;
      case '-': result = display_SendCommand(DI_CMD_DISPLAY_OFF); break;
      case '+': result = display_SendCommand(DI_CMD_DISPLAY_ON); break;
      case 'i': result = display_SendCommand(inverse ? DI_CMD_DISPLAY_INV_ON : DI_CMD_DISPLAY_INV_OFF); inverse = inverse ? 0 : 1; break;
      case 't': enter_display_type_mode(); break;
      case 'c': result = display_ClearScreen(); break;
    }

    printf("result(%i): %s\n", result, display_TranslateResult(result));
  }
}

/// <summary>Enters the steprocker parameter mode, where parameters can be get/set.</summary>
void enter_steprocker_parameter_mode()
{
  char c;

  StepRockerResult result;
  StatusCode status;
  int value;

  while(1)
  {
    printf("StepRocker Parameter mode: (q)uit, (g)et, (s)et, s(t)ore, (r)estore\n");

    value = 0;

    c = terminal_getchar();

    switch (c)
    {
      case 27:
      case 'q': return;
      case 'g': 
        result = steprocker_GetAxisParameter(SR_AXIS_PARAM_ACTUAL_POSITION, &value, &status); 
        printf("value: %u ", value);
        break;
    }

    printf("result(%i): %s, status(%i): %s\n", 
      result, steprocker_TranslateResult(result), 
      status, steprocker_TranslateStatusCode(status));
  }
}

/// <summary>Enters the general steprocker mode</summary>
void enter_steprocker_mode()
{
  char c;
  int velocity = 500;
  StepRockerRFSType rfs = RFS_START;
  int position = 500;

  StepRockerResult result;
  StatusCode status;

  while (1)
  {
    printf("StepRocker mode: (q)uit, (l)eft, (r)ight, (s)top, r(e)ferencesearch, moveto(p)osition\n");

    c = terminal_getchar();

    switch (c)
    {
      case 27:
      case 'q': return;
      case 's': result = steprocker_MotorStop(&status); break;
      case 'r': result = steprocker_RotateRight(velocity, &status); break;
      case 'l': result = steprocker_RotateLeft(velocity, &status); break;
      case 'e': result = steprocker_ReferenceSearch(rfs, &status); rfs = (rfs == RFS_START ? RFS_STOP : RFS_START);  break;
      case 'p': result = steprocker_MoveToPosition(MVP_RELATIVE, position, &status); break;
    }

    printf("result(%i): %s, status(%i): %s\n", 
      result, steprocker_TranslateResult(result), 
      status, steprocker_TranslateStatusCode(status));
  }

}

/// <summary>Long running call that sets the outputs and therefore the leds.</summary>
void execute_led_demo()
{
  int time  = 1000000 * 2;

  raspicomm_ClearOutputs( LED1 | LED2 );

  raspicomm_SetOutputs( LED1 );
  usleep(time);

  raspicomm_ClearOutputs( LED1 );
  raspicomm_SetOutputs( LED2 );
  usleep(time);
    
  raspicomm_SetOutputs( LED1 );
  usleep(time);

  raspicomm_ClearOutputs( LED1 | LED2 );
}

/// <summary>Demos the display, by showing 'hello world'</summary>
void execute_display_demo()
{
  /* clear the display */
  display_ClearScreen();

  /* enable the display */
  display_SendCommand(DI_CMD_DISPLAY_ON);

  /* Move the cursor to the first column and first row */
  display_SetXY(0, 0);

  /* Display the hello world text */
  display_PrintString("Hello World!");
}

/// <summary>Executes the supplied internal command</summary>
/// <param name="c">The command to execute</param>
void execute_command(DEMOCMD c, char* arg)
{
  ApiResult error;
  time_t time;
  Buttons buttons;
  Outputs outputs;  

  switch (c)
  {

  case CMD_REVISION:
    /* ignore, we handle this get_revision() only */
    break;

    /* START general commands */
    case CMD_INTERACTIVE:
      enter_interactive_mode();
      break;


    case CMD_QUIT:    
      printf("quitting...\n"); 
      exit(0);
      return;

    case CMD_HELP: /* help */
      print_help();
      break;
    /* END general commands */


    /* START lcd display commands */
    case CMD_LCD_DEMO:
      execute_display_demo();
      break;        

    case CMD_TYPE2DISPLAY: 
      enter_display_type_mode(); 
      break;

    case CMD_DISPLAY_MODE:
      enter_display_mode();
      break;
    /* END lcd display commands */

    /* START steprocker commands */
    case CMD_STEPROCKER:
      enter_steprocker_mode();
      break; 

    case CMD_STEPROCKER_PARAM:
      enter_steprocker_parameter_mode();
      break;
    /* END steprocker commands */

    /* START I/O commands */
    case CMD_GETOUTPUTS:
      error = raspicomm_GetOutputs(&outputs);
      printf("raspicomm_GetOutputs(): %s %i\n", raspicomm_TranslateResult(error), outputs);
      printf("outputs: LED1=%i, LED2=%i\n", 
              (outputs & LED1) ? 1 : 0,
              (outputs & LED2) ? 1 : 0 );
      break;

    case CMD_SETOUTPUTS:
      error = raspicomm_SetOutputs(LED1 | LED2);
      printf("raspicomm_SetOutputs(LED1 | LED2): %s\n", raspicomm_TranslateResult(error));
      break;

    case CMD_UNSETOUTPUTS:
      error = raspicomm_ClearOutputs(LED1 | LED2);
      printf("raspicomm_SetOutputs(LED1 | LED2): %s\n", raspicomm_TranslateResult(error));
      break;

    case CMD_GETBUTTONS:
      error = raspicomm_GetButtons(&buttons);
      printf("raspi+-comm_GetButtons(): %s %i\n", raspicomm_TranslateResult(error), buttons);
      printf("buttons: Left=%i Up=%i Right=%i Down=%i Pressed=%i\n", 
              (buttons & Left) ? 1 : 0, 
              (buttons & Up) ? 1 : 0,
              (buttons & Right) ? 1 : 0 ,
              (buttons & Down) ? 1 : 0, 
              (buttons & Pressed) ? 1 : 0
            );
      break;

    case CMD_LED_DEMO:
      execute_led_demo();
      break;
    /* END I/O commands */


    /* handle unknown command */
    default:
      printf("unknown command: %c\n", c); 
      break;
  
  }
 
}

/// <summary>Prints the interactive help to the standard output</summary>
void print_interactive_help(void)
{
  printf("options: (q)uit, (s)etoutput 1, (u)nsetputput 1, (g)etoutput 1, get(b)uttons, step(r)ocker, (p)arameter, (d)isplay\n");
}


/// <summary>Enters the main interactive loop, is executed when started without parameters</summary>
void enter_interactive_mode(void)
{
  int c; 
  DEMOCMD cmd;

  while(1)
  {
    /* show the user a hint what can be entered */
    print_interactive_help();

    /* get a keyboard character from the user */
    c = terminal_getchar();

    /* match the user input to the specific cmd */
    switch (c)
    {
      case 27:
      case 'q': cmd = CMD_QUIT; break;
      case 's': cmd = CMD_SETOUTPUTS; break;
      case 'g': cmd = CMD_GETOUTPUTS; break;
      case 'u': cmd = CMD_UNSETOUTPUTS; break;
      case 'b': cmd = CMD_GETBUTTONS; break;
      case 'r': cmd = CMD_STEPROCKER; break;
      case 'p': cmd = CMD_STEPROCKER_PARAM; break;
      case 'd': cmd = CMD_DISPLAY_MODE; break;
    }

    /* execute the entered command, if valid */
    execute_command(cmd, NULL);
  }
}

/// <summary>Handles the supplied commandline arguments.</summary>
void handle_arguments(int argc, char** argv)
{
  int c = -1;

  /* This option set a flag. */
  struct option long_options[] =
  {
    {"revision",     optional_argument, 0, 0 },
    {"interactive",  no_argument,       0, CMD_INTERACTIVE },
    {"help",         no_argument,       0, CMD_HELP },
    {"getoutputs",   no_argument,       0, CMD_GETOUTPUTS },
    {"setoutputs",   no_argument,       0, CMD_SETOUTPUTS },
    {"unsetoutputs", no_argument,       0, CMD_UNSETOUTPUTS },
    {"getbuttons",	 no_argument,		    0, CMD_GETBUTTONS },
    {"leddemo",	     no_argument,		    0, CMD_LED_DEMO },
    {"lcddemo",      no_argument,       0, CMD_LCD_DEMO },
    {"display",	     no_argument,		    0, CMD_TYPE2DISPLAY },
    {0,              0,                 0, 0}
  };

  /* enable errod messages for arguments */
  opterr = 1; 

  /* getopt_long stores the option index here. */
  int option_index = 0;

  do
  {
    c = getopt_long (argc, argv, "",
      long_options, &option_index);

    /* Detect the end of the options. */
    if (c != -1)
      execute_command(c, optarg);

  } while (c != -1);
  
}

Revision get_revision(int argc, char** argv)
{
  static struct option long_options[] = 
  {
    { "revision", optional_argument, 0, 0 },
    { 0,          0,                 0, 0 }
  };

  int option_index = 0;

  /* disable error messages for arguments */
  opterr = 0;

  int c = -1;

  // use the default revision as the default :-p
  Revision revision = REVISION_DEFAULT;

  do
  {
    c = getopt_long( argc, argv, "", long_options, &option_index);

    // if the user entered a revision
    if (c != -1 && optarg != NULL)
    {
      // retrieve the number
      int rev = atoi(optarg);

      // and sanitize the revision
      if (rev != REVISION_DEFAULT)
      {
        if (rev < REVISION_FIRST)
          rev = REVISION_FIRST;
        else if (REVISION_LATEST < rev)
          rev = REVISION_LATEST;
      }

      revision = rev;
    }
  } while(c != -1);

  /* reset the index back to start, so that the next getopt() call starts from the beginning */
  optind = 1;
  
  return revision;
}

/// <summary>Main Entry Point of Demo Application.</summary>
void main(int argc, char** argv)
{
  /* retrieve the revision */
  Revision revision = get_revision(argc, argv);

  /* initialize the raspicomm api */
  ApiResult result = raspicomm_Init(revision);

  /* retrieve the raspberry pi revision that is used in the library */
  raspicomm_GetRaspberryPiRevision(&revision);

  /* show the revision we are using */
  printf("using raspberrypi revision %i\n", revision);
  
  /* initialize the steprocker api */
  StepRockerResult srresult = steprocker_Init();
  
  /* initialize the display api */
  DisplayResult diresult = display_Init(0x3c);

  /* handle commandline arguments, if any */
  handle_arguments(argc, argv);

  /* uncomment for troubleshooting */
  //printf("Calling raspicomm_Init(): ");
  //printf( "%i %s\n", result, raspicomm_TranslateResult(result) );
  //printf("Calling steprocker_Init(): ");
  //printf( "%i %s\n", srresult, steprocker_TranslateResult(srresult) );
  //printf("Calling display_Init(): ");
  //printf( "%i %s\n", diresult, display_TranslateResult(diresult) );

  /* if no arguments where used, print the help */
  if (argc < 2)
    print_help();

  /*printf("Calling Shutdown(): ");*/
  result = raspicomm_Shutdown();
  /*printf( "%i %s\n", result, raspicomm_TranslateResult(result) );*/

}
