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
   SYSAHBCLKCTRL |= (1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<6 | 1<<7 | 1<<10 | 1<<14 | 1<<18); //enable clock for ROM, RAM0_1, FLASHREG, FLASH, GPIO, SWM, MRT, USART0, IOCON
   PRESETCTRL |= (1<<2 | 1<<3 | 1<<7 | 1<<10 | 1<<11); //clear USART FRG, USART0, MRT, GPIO, flash controller reset

   PINENABLE0 |= (1<<3 | 1<<16); //ACMP_I4 and ADC_3 disabled on pin PIO0_23
   PIO0_23 = 0<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13; //no pu/pd, disable hysteresis, input not inverted, disable od, bypass input filter, peripheral clock divider 0
   DIR0 |= (1<<23); //set output direction

   OS_Init(NUMTHREADS,"led",5,Task_Led,"idle",31,Task_Idle);
   OS_Start();
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
   while(1) {
      NOT0 = (1<<23);
      OS_Sleep(1000);
   }
}

void Task_Idle(void) {
   while(1) {
      WaitForInterrupt();
   }
}
