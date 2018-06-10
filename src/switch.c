#include "switch.h"
#include "os.h"
#include "lpc824.h"

extern int smphr_switch;

struct Switch_Data switch_data;

void Switch_Init(void) {
   PINENABLE0 |= (1<<4); //SWCLK disabled on PIO0_3
   PIO0_3 = (1<<3 | 1<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //pull-down resistor enabled, hysteresis enabled, input not inverted, open-drain mode disabled, bypass input filter, IOCONCLKDIV0
   DIR0 &= (~(1<<3)); //direction is input
   PINTSEL0 = (3<<0); //PIO0_3 selected for pin interrupt
   ISEL &= (~(1<<0)); //pin interrupt selected in PINTSEL0 is edge sensitive
   IST = (1<<0); //clear rising and falling edge detection for pin selected in PINTSEL0
   SIENR = (1<<0); //enable rising edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENR register
   SIENF = (1<<0); //enable falling edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENF register
   IPR6 = (IPR6&(~(3<<6))) | (0<<6); //PININT0 priority 0 (0 = highest, 3 = lowest)
   ISER0 = (1<<24); //enable PININT0
}
void PININT0_IRQHandler(void) {
   CIENR = (1<<0); //disable rising edge interrupt for pin selected in PINTSEL0
   CIENF = (1<<0); //disable falling edge interrupt for pin selected in PINTSEL0
   if(RISE&(1<<0)) { //rising edge has been detected
      RISE = (1<<0); //clear detection
      switch_data.edge = 'r';
   }
   else if(FALL&(1<<0)) { //falling edge has been detected
      FALL = (1<<0); //clear detection
      switch_data.edge = 'f';
   }
   OS_Blocking_Signal(&smphr_switch);
   OS_Suspend();
}
