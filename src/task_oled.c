#include "task_oled.h"
#include "fifos.h"
#include "os.h"
//#include "task_ds18b20.h"
#include "mrt.h"
#include "u8g2.h"
#include "utils.h"
#include "lpc824.h"

uint8_t u8x8_gpio_and_delay(u8x8_t*,uint8_t,uint8_t,void*);

extern int mtx_mrt1;
extern volatile int adc;
//extern struct Task_DS18B20_Data task_ds18b20_data;

u8g2_t u8g2; //a structure which contains all the data for one display

void Task_Oled(void) {
   char buf[64];
   double v;

   Fifo_Uart0_Put("Task_Oled has started",0);
   OS_Blocking_Wait(&mtx_mrt1);
   u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0,u8x8_byte_sw_i2c,u8x8_gpio_and_delay); //init u8g2 structure
   u8g2_InitDisplay(&u8g2); //send init sequence to the display, display is in sleep mode
   u8g2_SetPowerSave(&u8g2,0); //wake up display
   OS_Blocking_Signal(&mtx_mrt1);
   while(1) {
      OS_Blocking_Wait(&mtx_mrt1);

      u8g2_ClearBuffer(&u8g2);

      u8g2_DrawLine(&u8g2,0,0,127,0);
      u8g2_DrawLine(&u8g2,0,63,127,63);
      u8g2_DrawLine(&u8g2,0,0,0,63);
      u8g2_DrawLine(&u8g2,127,0,127,63);
      
      u8g2_DrawCircle(&u8g2,32,32,31,U8G2_DRAW_ALL);

      u8g2_SetFont(&u8g2,u8g2_font_8x13B_tf);
      v = adc/4095.0*3.3;
      mysprintf(buf,"%f2V",(char*)&v);
      u8g2_DrawStr(&u8g2,80,31,buf);

      //u8g2_SetFont(&u8g2,u8g2_font_8x13B_tf);
      //v=task_ds18b20_data.t[0]/10.0;
      //mysprintf(buf,"%f1%s",(char*)&v,"\xb0""C");
      //u8g2_DrawStr(&u8g2,8,101,buf);

      u8g2_SendBuffer(&u8g2);

      OS_Blocking_Signal(&mtx_mrt1);
      OS_Sleep(1500);
   }
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8,uint8_t msg,uint8_t arg_int,void *arg_ptr) {
   switch(msg) {
      case U8X8_MSG_GPIO_AND_DELAY_INIT: //called once during init phase of u8g2; can be used to setup pins
         PINENABLE0 |= (1<<1 | 1<<9); //ACMP_I2 and CLKIN disabled on pin PIO0_1
         //SDA
         PIO0_1 = (0<<3 | 0<<5 | 0<<6 | 1<<10 | 0<<11); //no pd/pu, hysteresis disable, input not inverted, open drain mode, bypass input filter
         //SCL
         PIO0_15 = (0<<3 | 0<<5 | 0<<6 | 1<<10 | 0<<11); //no pd/pu, hysteresis disable, input not inverted, open drain mode, bypass input filter
         //direction is output
         DIR0 |= (1<<1 | 1<<15);
         //initially released
         SET0 = (1<<1 | 1<<15);
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
         if(arg_int==0) CLR0 = (1<<15);
         else SET0 = (1<<15);
         break;
      case U8X8_MSG_GPIO_I2C_DATA: //arg_int=0: output low at I2C data pin; arg_int=1: input dir with pullup high for I2C data pin
         if(arg_int==0) CLR0 = (1<<1);
         else SET0 = (1<<1);
         break;
      default:
         u8x8_SetGPIOResult(u8x8,1); //default return value
         break;
   }
   return 1;
}
