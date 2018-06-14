#include "output.h"
#include "fifos.h"
#include "os.h"
#include "lpc824.h"

struct Output_Data output_data;

void output(char *buf, enum eOutputSubsystem subsystem, enum eOutputLevel level, int block) {
   int smphrFinished;
   if(level & output_data.mask[subsystem]) {
      if(block) {
         OS_InitSemaphore(&smphrFinished,0);
         Fifo_Uart0_Put(buf, &smphrFinished);
         OS_Blocking_Wait(&smphrFinished);
      }
      else
         Fifo_Uart0_Put(buf, 0);
   }
}
