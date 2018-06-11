#ifndef __SWITCH_H__
#define __SWITCH_H__

struct Dump {
   volatile int index;
   volatile long long int millis[100];
};

struct Switch_Data {
   volatile int active;
};

void Switch_Init(void);
int Switch_Pressed(void);

#endif
