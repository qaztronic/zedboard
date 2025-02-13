// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sys_cmd.h"

int main()
{
  init_platform();
  
  print("\r\n");
  print("------------------------------------ \\  \\\{|}/  /   \\  \\\{|}/  /    \\  \\\{|}/  /    \r\n");
  print(" Command Line Interface Terminal ...  \\   *   /     \\   *   /      \\   *   /       \r\n");
  print("------------------------------------   -..|..-       -..|..-        -..|..-         \r\n");
  
  sys_cli_task();
  
  cleanup_platform();
  return 0;
}
 