#include "switch.h"
#include "main.h"
#include "os.h"
#include "utils-asm.h"
#include "lpc824.h"

extern int smphr_switch;
extern volatile long long int millis;

volatile struct Switch_Data switch_data;

void Switch_Init(void) {
#if BOARD == BOARD_TEST
   PINENABLE0 |= (1<<22); //ADC_9 disabled on PIO0_17
   PIO0_17 = (2<<3 | 1<<5 | 1<<6 | 0<<10 | 0<<11 | 0<<13); //pull-up resistor enabled, hysteresis enabled, input inverted, open-drain mode disabled, bypass input filter, IOCONCLKDIV0
   DIR0 &= (~(1<<17)); //direction is input
   PINTSEL0 = (17<<0); //PIO0_17 selected for pin interrupt
#elif BOARD == BOARD_RELEASE
   PINENABLE0 |= (1<<4); //SWCLK disabled on PIO0_3
   PIO0_3 = (1<<3 | 1<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //pull-down resistor enabled, hysteresis enabled, input not inverted, open-drain mode disabled, bypass input filter, IOCONCLKDIV0
   DIR0 &= (~(1<<3)); //direction is input
   PINTSEL0 = (3<<0); //PIO0_3 selected for pin interrupt
#endif
   ISEL &= (~(1<<0)); //pin interrupt selected in PINTSEL0 is edge sensitive
   IST = (1<<0); //clear rising and falling edge detection for pin selected in PINTSEL0
   SIENR = (1<<0); //enable rising edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENR register
   CIENF = (1<<0); //disable falling edge interrupt for pin selected in PINTSEL0
   IPR6 = (IPR6&(~(3<<6))) | (1<<6); //PININT0 priority 1 (0 = highest, 3 = lowest)
   ISER0 = (1<<24); //enable PININT0
}

int Switch_Pressed(void) {
#if BOARD == BOARD_TEST
   return (PIN0>>17)&1;
#elif BOARD == BOARD_RELEASE
   return (PIN0>>3)&1;
#endif
}

void PININT0_IRQHandler(void) {
   CIENR = (1<<0); //disable rising edge interrupt for pin selected in PINTSEL0
   RISE = (1<<0); //clear rising edge detection
   if(switch_data.active==0) {
      switch_data.active = 1;
      switch_data.delay = 0;
      switch_data.start = millis;
      Task_Blocking_Signal(&smphr_switch);
      Task_Suspend();
   }
}
