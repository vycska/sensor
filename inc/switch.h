#ifndef __SWITCH_H__
#define __SWITCH_H__

struct Dump {
   int index;
   long long int millis[20];
};

struct Switch_Data {
   volatile int active,delay,duration;
   volatile long long int start;
};

void Switch_Init(void);
int Switch_Pressed(void);

#endif
