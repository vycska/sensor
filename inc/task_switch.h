#ifndef __TASK_SWITCH_H__
#define __TASK_SWITCH_H__

struct Task_Switch_Data {
   char active;
   long long int start,duration;
};

void Task_Switch(void);

#endif
