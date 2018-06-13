#include "task_oled.h"
#include "adc.h"
#include "fifos.h"
#include "os.h"
#include "task_bme280.h"
#include "task_ds18b20.h"
#include "task_switch.h"
#include "mrt.h"
#include "u8g2.h"
#include "utils-asm.h"
#include "utils.h"
#include "lpc824.h"

uint8_t u8x8_gpio_and_delay(u8x8_t*,uint8_t,uint8_t,void*);

extern volatile long long int millis;
extern struct tcb *RunPt;
extern struct ADC_Data adc_data;
extern struct Task_BME280_Data task_bme280_data;
extern struct Task_DS18B20_Data task_ds18b20_data;

struct Task_Oled_Data task_oled_data = {0,5};
u8g2_t u8g2; //a structure which contains all the data for one display

void Task_Oled(void) {
   char buf[96],buf2[16];
   double v;

   Fifo_Uart0_Put("Task_Oled has started",0);

   DisableInterrupts();
   u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0,u8x8_byte_sw_i2c,u8x8_gpio_and_delay); //init u8g2 structure
   u8g2_InitDisplay(&u8g2); //send init sequence to the display, display is in sleep mode
   u8g2_SetPowerSave(&u8g2,0); //wake up display
   EnableInterrupts();

   u8g2_ClearBuffer(&u8g2);

   u8g2_DrawLine(&u8g2,0,0,127,0);
   u8g2_DrawLine(&u8g2,0,63,127,63);
   u8g2_DrawLine(&u8g2,0,0,0,63);
   u8g2_DrawLine(&u8g2,127,0,127,63);
   u8g2_DrawCircle(&u8g2,64,32,31,U8G2_DRAW_ALL);
   u8g2_SendBuffer(&u8g2);

   OS_Sleep(2000);

   while(1) {
      u8g2_ClearBuffer(&u8g2);

      switch(task_oled_data.screen) {
         case 0:
            mysprintf(buf,"ds18b20: temper., %s","\xb0""C");
            if(task_ds18b20_data.temperature!=DS18B20_ERROR_TEMPERATURE) {
               v=task_ds18b20_data.temperature/100.0;
               mysprintf(buf2,"%f1",(char*)&v);
            }
            else
               mysprintf(buf2,"**.*");
            break;
         case 1:
            mysprintf(buf,"bme280: temper., %s","\xb0""C");
            v = task_bme280_data.t/100.0;
            mysprintf(buf2,"%f1",(char*)&v);
            break;
         case 2:
            mysprintf(buf,"bme280: dregme, %%");
            v = task_bme280_data.h/100.0;
            mysprintf(buf2,"%f1",(char*)&v);
            break;
         case 3:
            mysprintf(buf,"bme280: slegis, mmHg");
            v = task_bme280_data.p/100.0;
            mysprintf(buf2,"%f1",(char*)&v);
            break;
         case 4: //adc
            mysprintf(buf,"Li-Ion baterija, V");
            v = ((double)adc_data.sum/adc_data.count)/4095.0*3.3 * 2;
            mysprintf(buf2,"%f2",(char*)&v);
            break;
      }
      u8g2_SetFont(&u8g2,u8g2_font_6x10_tf);
      u8g2_DrawStr(&u8g2,0,8,buf);
      u8g2_SetFont(&u8g2,u8g2_font_fur35_tn);
      u8g2_DrawStr(&u8g2,2,62,buf2);

      u8g2_SendBuffer(&u8g2);

      OS_Sleep(500);
   }
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8,uint8_t msg,uint8_t arg_int,void *arg_ptr) {
   switch(msg) {
      case U8X8_MSG_GPIO_AND_DELAY_INIT: //called once during init phase of u8g2; can be used to setup pins
         PINENABLE0 |= (1<<22 | 1<<23); //ADC_9 disabled on PIO0_17, ADC_10 disabled on PIO0_13
         //SDA is PIO0_17
         PIO0_17 = (0<<3 | 0<<5 | 0<<6 | 1<<10 | 0<<11 | 0<<13); //no pd/pu, hysteresis disable, input not inverted, open drain mode, bypass input filter, IOCONCLKDIV0
         //SCL is PIO0_13
         PIO0_13 = (0<<3 | 0<<5 | 0<<6 | 1<<10 | 0<<11 | 0<<13); //no pd/pu, hysteresis disable, input not inverted, open drain mode, bypass input filter, IOCONCLKDIV0
         //direction is output
         DIR0 |= (1<<13 | 1<<17);
         //initially released
         SET0 = (1<<13 | 1<<17);
         break;
      case U8X8_MSG_DELAY_100NANO: //delay arg_int*100 nano seconds
         MRT1_Delay(arg_int*100);
         break;
      case U8X8_MSG_DELAY_10MICRO: //delay arg_int * 10 micro seconds
         MRT1_Delay(arg_int*10*1000);
         break;
      case U8X8_MSG_DELAY_MILLI: //delay arg_int*1 milli second
         MRT1_Delay(arg_int*1000*1000);
         break;
      case U8X8_MSG_DELAY_I2C: //arg_int is the I2C speed in 100 kHz, e.g. 4=400 Hz; arg_int=1: delay by 5 us, arg_int=4: delay by 1.25 us
         MRT1_Delay(5.0/arg_int*1000);
         break;
      case U8X8_MSG_GPIO_I2C_CLOCK: //arg_int=0: output low at I2C clock pin; arg_int=1: input dir with pullup high for I2C clock pin
         if(arg_int==0) CLR0 = (1<<13);
         else SET0 = (1<<13);
         break;
      case U8X8_MSG_GPIO_I2C_DATA: //arg_int=0: output low at I2C data pin; arg_int=1: input dir with pullup high for I2C data pin
         if(arg_int==0) CLR0 = (1<<17);
         else SET0 = (1<<17);
         break;
      default:
         u8x8_SetGPIOResult(u8x8,1); //default return value
         break;
   }
   return 1;
}
