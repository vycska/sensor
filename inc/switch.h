#ifndef __SWITCH_H__
#define __SWITCH_H__

struct Switch_Data {
   volatile char edge;
};

void Switch_Init(void);

#endif
