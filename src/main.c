#include "main.h"
#include "adc.h"
#include "fifos.h"
#include "mrt.h"
#include "os.h"
#include "os-asm.h"
#include "pll.h"
#include "task_adc.h"
#include "task_command_parser.h"
#include "task_ds18b20.h"
#include "task_idle.h"
#include "task_oled.h"
#include "task_uart0_output.h"
#include "uart.h"
#include "utils-asm.h"
#include "lpc824.h"

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;

int mtx_mrt1;
volatile long long int millis;

void main(void) {
   PDRUNCFG &= (~(1<<0 | 1<<1 | 1<<2 | 1<<4 | 1<<7)); //IRC output, IRC, flash, ADC, PLL powered
   PLL_Init();
   SYSAHBCLKCTRL |= (1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<6 | 1<<7 | 1<<10 | 1<<14 | 1<<18 | 1<<24); //enable clock for ROM, RAM0_1, FLASHREG, FLASH, GPIO, SWM, MRT, USART0, IOCON, ADC
   PRESETCTRL |= (1<<2 | 1<<3 | 1<<7 | 1<<10 | 1<<11); //clear USART FRG, USART0, MRT, GPIO, flash controller reset

   ADC_Init();
   UART0_Init();
   MRT2_Init(1000);

   PINENABLE0 |= (1<<3 | 1<<16); //ACMP_I4 and ADC_3 disabled on pin PIO0_23
   PIO0_23 = (0<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //no pu/pd, disable hysteresis, input not inverted, disable od, bypass input filter, peripheral clock divider 0
   DIR0 |= (1<<23); //set output direction

   Fifo_Uart0_Init();
   Fifo_Command_Parser_Init();

   OS_InitSemaphore(&mtx_mrt1,1);

   OS_Init(NUMTHREADS,
         "uart0_output",8,Task_Uart0_Output,
         "adc",7,Task_ADC,
         "ds18b20",6,Task_DS18B20,
         "command_parser",9,Task_Command_Parser,
         "oled",6,Task_Oled,
         "idle",31,Task_Idle);
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
