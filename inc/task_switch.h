#ifndef __TASK_SWITCH_H__
#define __TASK_SWITCH_H__

struct Task_Switch_Data {
   volatile int duration;
   volatile long long int start;
};

void Task_Switch(void);

#endif
