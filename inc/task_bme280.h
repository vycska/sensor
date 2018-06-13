#ifndef __TASK_BME280_H__
#define __TASK_BME280_H__

#define BME280_ERROR_VALUE (-10000)

struct Task_BME280_Data {
   int h, p, t;
};

void Task_BME280(void);

#endif
