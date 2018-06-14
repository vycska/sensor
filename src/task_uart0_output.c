#include "task_uart0_output.h"
#include "fifos.h"
#include "os.h"
#include "output.h"
#include "uart.h"
#include <string.h>

void Task_Uart0_Output(void) {
   char *pString;
   int *smphrFinished = 0;

   output("Task_Uart0_Output has started", eOutputSubsystemSystem, eOutputLevelNormal, 0);

   while(1) {
      Fifo_Uart0_Get(&pString, &smphrFinished);
      UART0_Transmit(pString, strlen(pString));
      if(smphrFinished)
         OS_Blocking_Signal(smphrFinished);
   }
}
