#include "adc.h"
#include "main.h"
#include "lpc824.h"

volatile int adc;

void ADC_Init(void) {
   PIO0_14 = 0;
   PINENABLE0 &= (~(1<<15)); //ADC_2 enabled on PIO0_14
   ADCTRM &= (~(1<<5)); //VDD = 2.7V to 3.6V
   //calibration
   ADCCTRL = ((CLOCK*2)<<0 | 0<<10 | 1<<30); //set calibration mode, divider for 500kHz, clear LPWR
   while(ADCCTRL&(1<<30));
   ADCCTRL = (0<<0 | 1<<10); //divider is 1, set low power mode
   ADCSEQA_CTRL = (1<<2 | 0<<12 | 1<<30 | 1u<<31); //select channel 2, conversions are initiated by software, end of sequence mode, sequence enabled
   //select channel 2
   //set TRIGPOL and SEQ_ENA
   //set start
   //read results
   //IPR4 = (IPR4&(~(3<<6))) | (2<<6); //interrupt priority 2
   //ISER0 = (1<<16); //ADC_SEQA interrupt enable
}

/*
void ADC_SEQA_IRQHandler(void) {

}
*/
