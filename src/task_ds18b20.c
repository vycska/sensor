#include "task_ds18b20.h"
#include "ds18b20.h"
#include "fifos.h"
#include "onewire.h"
#include "os.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <string.h>

struct Task_DS18B20_Data task_ds18b20_data = {DS18B20_ERROR_VALUE};

void Task_DS18B20(void) {
   char s[80];
   unsigned char data[9];
   int i,k,l,smphrFinished;

   Fifo_Uart0_Put("Task_DS18B20 has started", 0);

   DS18B20_Init();

   OS_InitSemaphore(&smphrFinished, 0);

   k = DS18B20_ReadROM(data);

   if(k == DS18B20_OK) {
      l = mysprintf(s, "one-wire device: ");
      for(i = 0; i < 8; i++)
         l += mysprintf(&s[l], "0x%x%s", (unsigned int)data[i], i == 7 ? "." : " ");
      Fifo_Uart0_Put(s, &smphrFinished);
      OS_Blocking_Wait(&smphrFinished);
   }

   while(1) {
      DS18B20_ConvertTAll();
      OS_Sleep(750); //conversion time
      k = DS18B20_ReadScratchpad(0,data);
      task_ds18b20_data.temperature = (k==DS18B20_OK ? DS18B20_GetTemperature(data)*100 : DS18B20_ERROR_VALUE);
      OS_Sleep(1000);
   }
}
