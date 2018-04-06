#include "pll.h"
#include "lpc824.h"

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;

void main(void) {
   int i;

   //PLL_Init();

   while(1) {
      for(i=0;i<1000000;i++);
   }
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
