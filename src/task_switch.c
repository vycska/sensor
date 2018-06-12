#include "task_switch.h"
#include "fifos.h"
#include "main.h"
#include "os.h"
#include "switch.h"
#include "task_oled.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"

extern volatile long long int millis;
extern volatile struct Switch_Data switch_data;
extern struct Task_Oled_Data task_oled_data;

int smphr_switch;

void Task_Switch(void) {
   char buf[32];

   Fifo_Uart0_Put("Task_Switch has started", 0);

   Switch_Init();

   OS_InitSemaphore(&smphr_switch, 0);

   while(1) {
      OS_Blocking_Wait(&smphr_switch);
      switch(switch_data.active) {
         case 1:
            task_oled_data.screen = (task_oled_data.screen+1)%task_oled_data.total_screens;
            break;
         case 0:
            if(switch_data.duration>=1000) {
               mysprintf(buf,"switch %d",switch_data.duration);
               Fifo_Command_Parser_Put(buf);
            }
            OS_Sleep(100);
            SIENR = (1<<0); //enable rising edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENR register
            break;
      }
   }
}
