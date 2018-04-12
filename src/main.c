#include "main.h"
#include "os.h"
#include "os-asm.h"
#include "pll.h"
#include "utils-asm.h"
#include "lpc824.h"

void Task_Led(void);
void Task_Idle(void);

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;

volatile long long int millis;

void main(void) {
   PLL_Init();
   SYSAHBCLKCTRL |= (1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<6 | 1<<7 | 1<<10 | 1<<18); //enable clock for ROM, RAM0_1, FLASHREG, FLASH, GPIO, MRT, IOCON
   PRESETCTRL |= (1<<7 | 1<<10 | 1<<11); //clear MRT, GPIO, flash controller reset

   OS_Init(NUMTHREADS,
           "led",5,Task_Led,
           "idle",31,Task_Idle);

   OS_Start();
   while(1);
}

void init(void) {
   char *dst, *src;

   //copy data to ram
   for(src = &_data_start_lma, dst = &_data_start; dst < &_data_end; src++, dst++)
      *dst = *src;
   //zero bss
   for(dst = &_bss_start; dst < &_bss_end; dst++)
      *dst = 0;
}

void Task_Led(void) {
   int i;
   for(i=0;i<1000000;i++);
   OS_Sleep(2000);
}

void Task_Idle(void) {
   while(1) {
      WaitForInterrupt();
   }
}
