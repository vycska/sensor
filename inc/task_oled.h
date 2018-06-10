#ifndef __TASK_OLED_H__
#define __TASK_OLED_H__

struct Task_Oled_Data {
   volatile char screen;
   const char total_screens;
};

void Task_Oled(void);

#endif
