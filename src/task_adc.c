#include "task_adc.h"
#include "adc.h"
#include "fifos.h"
#include "os.h"
#include "utils.h"
#include "lpc824.h"

extern volatile int adc;

void Task_ADC(void) {
   char s[24];
   int smphrFinished;
   double value;

   Fifo_Uart0_Put("Task_ADC has started", 0);

   OS_InitSemaphore(&smphrFinished,0);

   //ADC_Init();

   while(1) {
      //adc = (ADCDAT2>>4)&0xfff; //read adc conversion result to clear flags [overrun and datavalid]
      //ADCSEQA_CTRL |= (1<<26); //start conversion
      //while((ADCDAT2&(1u<<31))==0); //wait for the completion flag
      //adc = (ADCDAT2>>4)&0xfff;
      if(adc>=0) {
         value = 3.3 * adc / 4095.0;
         mysprintf(s, "%u %f2V", adc, (char *)&value);
         Fifo_Uart0_Put(s,&smphrFinished);
      }
      OS_Sleep(1000);
   }
}
