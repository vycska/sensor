#include "main.h"
#include "adc.h"
#include "config.h"
#include "fifos.h"
#include "i2c.h"
#include "iap.h"
#include "led.h"
#include "mrt.h"
#include "output.h"
#include "os.h"
#include "os-asm.h"
#include "pll.h"
#include "task_bme280.h"
#include "task_command_parser.h"
#include "task_ds18b20.h"
#include "task_idle.h"
#include "task_oled.h"
#include "task_switch.h"
#include "task_uart_input.h"
#include "task_uart_output.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;
extern char _flash_start, _flash_end, _ram_start, _ram_end;
extern char _intvecs_size, _text_size, _rodata_size, _data_size, _bss_size, _stack_size, _heap_size;

int mtx_i2c0,mtx_mrt1;
volatile long long int millis;

void main(void) {
   char buf[32];
   int t; 

   t = config_load();

   PDRUNCFG &= (~(1<<0 | 1<<1 | 1<<2 | 1<<4 | 1<<7)); //IRC output, IRC, flash, ADC, PLL powered
   SYSAHBCLKCTRL |= (1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<10 | 1<<14 | 1<<18 | 1<<24); //enable clock for ROM, RAM0_1, FLASHREG, FLASH, I2C0, GPIO, SWM, MRT, USART0, IOCON, ADC
   PRESETCTRL |= (1<<2 | 1<<3 | 1<<6 | 1<<7 | 1<<10 | 1<<11); //clear USART FRG, USART0, I2C0, MRT, GPIO, flash controller reset

   PLL_Init();
   ADC_Init();
   I2C0_Init();
   LED_Init();
   MRT2_Init(1000);
   UART_Init();

   Fifo_Uart_Output_Init();
   Fifo_Command_Parser_Init();

   OS_InitSemaphore(&mtx_i2c0,1);
   OS_InitSemaphore(&mtx_mrt1,1);

   mysprintf(buf,"VERSION: %d",VERSION);
   output(buf,eOutputSubsystemSystem, eOutputLevelImportant, -1);

   output(__DATE__,eOutputSubsystemSystem, eOutputLevelImportant, -1);
   output(__TIME__,eOutputSubsystemSystem, eOutputLevelImportant, -1);

   output(t?"config_load error":"config_load ok", eOutputSubsystemSystem, eOutputLevelImportant, -1);

   mysprintf(buf, "_flash_start: %x [%u]", (unsigned int)&_flash_start,(unsigned int)&_flash_start);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_flash_end: %x [%u]", (unsigned int)&_flash_end,(unsigned int)&_flash_end);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_ram_start: %x [%u]", (unsigned int)&_ram_start,(unsigned int)&_ram_start);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_ram_end: %x [%u]", (unsigned int)&_ram_end,(unsigned int)&_ram_end);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_intvecs_size: %u", (unsigned int)&_intvecs_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_text_size: %u", (unsigned int)&_text_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_rodata_size: %u", (unsigned int)&_rodata_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_data_size: %u", (unsigned int)&_data_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_bss_size: %u", (unsigned int)&_bss_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_stack_size: %u", (unsigned int)&_stack_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   mysprintf(buf, "_heap_size: %u", (unsigned int)&_heap_size);
   output(buf, eOutputSubsystemSystem, eOutputLevelNormal, -1);

   OS_Init(NUMTHREADS,
         "switch",            3,    Task_Switch,
         "uart_input",        3,    Task_Uart_Input,
         "command_parser",    4,    Task_Command_Parser,
         "bme280",            6,    Task_BME280,
         "ds18b20",           6,    Task_DS18B20,
         "oled",              7,    Task_Oled,
         "uart_output",       8,    Task_Uart_Output,
         "idle",              31,   Task_Idle);
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
