#ifndef __ADC_H__
#define __ADC_H__

#define ADC_TOTAL_DATA (10)

struct ADC_Data {
   volatile short data[ADC_TOTAL_DATA];
   volatile int index,count,sum;
};

void ADC_Init(void);
void ADC_SEQA_IRQHandler(void);

#endif
