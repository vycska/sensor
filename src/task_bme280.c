#include "task_bme280.h"
#include "bme280.h"
#include "fifos.h"
#include "os.h"
#include "output.h"
#include "utils.h"
#include <string.h>

extern struct BME280_Data bme280_data;

struct Task_BME280_Data task_bme280_data = {BME280_ERROR_VALUE, BME280_ERROR_VALUE, BME280_ERROR_VALUE};

void Task_BME280(void) {
   char s[24];
   unsigned char id[1];
   double h, p, t;

   output("Task_BME280 has started", eOutputSubsystemSystem, eOutputLevelDebug, 0);

   Task_Sleep(2); //max time for first communication

   BME280_Init();

   BME280_GetID(id);

   mysprintf(s,"BME280 id: %x",(unsigned int)id[0]);
   output(s, eOutputSubsystemBME280, eOutputLevelDebug, 1);

   while(1) {
      BME280_StartForcedMeasurement();
      Task_Sleep(1.25 + 2.5 * power(2, bme280_data.os_t - 1) + 2.3 * power(2, bme280_data.os_p - 1) + 0.575 + 2.3 * power(2, bme280_data.os_h - 1) + 0.575);
      BME280_GetValue(&h, &p, &t);

      mysprintf(s, "T: %f1 C", (char *)&t);
      output(s, eOutputSubsystemBME280, eOutputLevelDebug, 1);
      mysprintf(s, "P: %f1 mmHg", (char *)&p);
      output(s, eOutputSubsystemBME280, eOutputLevelDebug, 1);
      mysprintf(s, "H: %f1 %%", (char *)&h);
      output(s, eOutputSubsystemBME280, eOutputLevelDebug, 1);

      task_bme280_data.h = h * 100;
      task_bme280_data.p = p * 100;
      task_bme280_data.t = t * 100;

      Task_Sleep(2000);
   }
}
