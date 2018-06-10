#include "task_switch.h"
#include "fifos.h"
#include "main.h"
#include "os.h"
#include "switch.h"
#include "task_oled.h"
#include "utils.h"
#include "lpc824.h"

extern volatile long long int millis;
extern struct Switch_Data switch_data;
extern struct Task_Oled_Data task_oled_data;

int smphr_switch;
struct Task_Switch_Data task_switch_data;

void Task_Switch(void) {
   Fifo_Uart0_Put("Task_Switch has started", 0);

   Switch_Init();

   OS_InitSemaphore(&smphr_switch, 0);

   while(1) {
      OS_Blocking_Wait(&smphr_switch);
      if(switch_data.edge=='r') {
         task_switch_data.start = millis;
         task_switch_data.active = 1;
      }
      else if(switch_data.edge=='f') {
         task_switch_data.duration = millis-task_switch_data.start;
         task_switch_data.active = 0;
         if(task_switch_data.duration>=100 && task_switch_data.duration<5000)
            task_oled_data.screen = (task_oled_data.screen+1)%task_oled_data.total_screens;
         //else if(task_switch_data.duration>=10000 && task_switch_data.duration<15000)
      }
      OS_Sleep(30); //switch may bounce during this period
      //enable interrupts
      SIENF = (1<<0); //enable falling edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENF register
      SIENR = (1<<0); //enable rising edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENR register
   }
}
