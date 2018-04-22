#include "task_ds18b20.h"
#include "ds18b20.h"
#include "fifos.h"
#include "onewire.h"
#include "os.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <string.h>

extern int mtx_mrt1;

struct Task_DS18B20_Data task_ds18b20_data;

void Task_DS18B20(void) {
   char s[64];
   unsigned char data[9];
   int i,k,l,smphrFinished;

   Fifo_Uart0_Put("Task_DS18B20 has started", 0);

   DS18B20_Init();

   OS_InitSemaphore(&smphrFinished, 0);

   OS_Blocking_Wait(&mtx_mrt1);
   k = DS18B20_ReadROM(data);
   OS_Blocking_Signal(&mtx_mrt1);

   l = mysprintf(s, "one-wire device: ");
   for(i = 0; i < 8; i++)
      l += mysprintf(&s[l], "0x%x%s", (unsigned int)data[i], i == 7 ? "." : " ");
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   while(1) {
      OS_Blocking_Wait(&mtx_mrt1);
      DS18B20_ConvertTAll();
      OS_Blocking_Signal(&mtx_mrt1);

      OS_Sleep(750); //conversion time

      OS_Blocking_Wait(&mtx_mrt1);
      k = DS18B20_ReadScratchpad(0,data);
      OS_Blocking_Signal(&mtx_mrt1);

      task_ds18b20_data.temperature = (k==DS18B20_OK ? DS18B20_GetTemperature(data)*100 : -10000);

      mysprintf(s,"temperature: %f2",task_ds18b20_data.temperature/100.0);
      Fifo_Uart0_Put(s,&smphrFinished);
      OS_Blocking_Wait(&smphrFinished);

      OS_Sleep(1000);
   }
}
