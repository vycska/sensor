#include "mrt.h"
#include "main.h"
#include "os.h"
#include "lpc824.h"

extern volatile long long int millis;
extern struct tcb *RunPt;

void MRT0_Init(void) {
   IPR2 = (IPR2&(~(3<<22))) | (0<<22); //priority 0 (highest)
   ISER0 = (1<<10); //interrupt enable
   CTRL0 = (1<<0 | 0<<1); //enable interrupt, repeat interrupt mode
   INTVAL0 = 1*CLOCK*1000 | 1u<<31; //load value for 1 ms, force load
}

void MRT_IRQHandler(void) {
   if(STAT0&(1<<0)) { //TIMER0 -- process sleeping threads
      struct tcb *cursor;
      STAT0 |= (1<<0); //clear the interrupt request
      millis += 1; //increment milliseconds timer
      //process sleeping threads
      for(cursor=RunPt->next;cursor!=RunPt;cursor=cursor->next)
         if(cursor->sleep > 0)
            cursor->sleep -= 1;
   }
   if(STAT1&(1<<0)) { //TIMER1
      STAT1 |= (1<<0);
   }
   if(STAT2&(1<<0)) { //TIMER2
      STAT2 |= (1<<0);
   }
   if(STAT3&(1<<0)) { //TIMER3
      STAT3 |= (1<<0);
   }
}
