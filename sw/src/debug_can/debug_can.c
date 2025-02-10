// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "platform.h"
#include "sys_cmd.h"

/*-----------------------------------------------------------*/
char func_can( const unsigned char argc, const char *argv[] )
{
  // unsigned int value = strtoul( argv[2], (char **)NULL, 16 );
  
  PRINTF_MACRO( "running func_can\r\n" );

  for(int i = 0; i < 5; i++ )
  {
    sleep(1);
    PRINTF_MACRO("... sleep test[%d]\r\n", i);
  }

  return EXIT_SUCCESS;
}
