#include "task_bme280.h"
#include "bme280.h"
#include "fifos.h"
#include "os.h"
#include "utils.h"
#include <string.h>

extern struct BME280_Config bme280_config;

struct Task_BME280_Data task_bme280_data;

void Task_BME280(void) {
   char s[24];
   unsigned char id[1];
   int smphrFinished;
   double h, p, t;

   Fifo_Uart0_Put("Task_BME280 has started", 0);

   OS_InitSemaphore(&smphrFinished, 0);

   OS_Sleep(2);                 //max time for first communication

   BME280_Init();

   BME280_GetID(id);

   mysprintf(s,"BME280 id: %x",(unsigned int)id[0]);
   Fifo_Uart0_Put(s,&smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   while(1) {
      BME280_StartForcedMeasurement();
      OS_Sleep(1.25 + 2.5 * power(2, bme280_config.os_t - 1) + 2.3 * power(2, bme280_config.os_p - 1) + 0.575 + 2.3 * power(2, bme280_config.os_h - 1) + 0.575);
      BME280_GetValue(&h, &p, &t);

      //mysprintf(s, "T: %f1 C", (char *)&t);
      //Fifo_Uart0_Put(s, &smphrFinished);
      //OS_Blocking_Wait(&smphrFinished);
      //mysprintf(s, "P: %f1 mmHg", (char *)&p);
      //Fifo_Uart0_Put(s, &smphrFinished);
      //OS_Blocking_Wait(&smphrFinished);
      //mysprintf(s, "H: %f1 %%", (char *)&h);
      //Fifo_Uart0_Put(s, &smphrFinished);
      //OS_Blocking_Wait(&smphrFinished);

      task_bme280_data.h = h * 100;
      task_bme280_data.p = p * 100;
      task_bme280_data.t = t * 100;

      OS_Sleep(2000);
   }
}
