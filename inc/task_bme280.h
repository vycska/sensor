#ifndef __TASK_BME280_H__
#define __TASK_BME280_H__

#define PA2MMHG (0.0075006) //1 atm [standard atmosphere] = 760 torr = 101325 Pa = 1.01325 bar; 1 mmHg = 133.322387415 Pa --> 1 Pa = 0.0075006 mmHg

struct Task_BME280_Data {
   int ready;
   double h, p, t,
          pbase;
};

void Task_BME280(void);

#endif
