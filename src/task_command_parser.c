#include "task_command_parser.h"
#include "bme280.h"
#include "config.h"
#include "fifos.h"
#include "iap.h"
#include "led.h"
#include "onewire.h"
#include "os.h"
#include "output.h"
#include "switch.h"
#include "task_bme280.h"
#include "task_oled.h"
#include "task_switch.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void params_fill(char*,unsigned int*);
int params_count(unsigned int*);
int params_integer(char,unsigned int*);

extern char _flash_start, _flash_end, _ram_start, _ram_end;
extern volatile long long int millis;
extern struct BME280_Data bme280_data;
extern struct LED_Data led_data;
extern struct Output_Data output_data;
extern volatile struct UART_Data uart_data;
extern struct Task_BME280_Data task_bme280_data;
extern struct Task_Oled_Data task_oled_data;
extern struct tcb *RunPt,tcbs[NUMTHREADS];


void Task_Command_Parser(void) {
   char *pString,buf[128];
   int i, l;
   unsigned int t, params[8];

   output("Task_Command_Parser has started", eOutputSubsystemSystem, eOutputLevelDebug, 0);

   while(1) {
      Fifo_Command_Parser_Get(&pString);
      mysprintf(buf, "<< %s >>", pString);
      output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

      memset(params, 0, sizeof(params));
      params_fill(pString, params);

      switch(crc16((unsigned char *)params[1], strlen((char *)params[1]))) {
         case 0x178a: //system_reset
            SystemReset();
            break;
         case 0x57e6: //millis
            mysprintf(buf, "%l", (char *)&millis);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            break;
         case 0xd89c: //live_time
            mysprintf(buf, "%d,%d:%d:%d", (int)(millis / 1000 / 60 / 60 / 24), (int)(millis / 1000 / 60 / 60 % 24), (int)(millis / 1000 / 60 % 60), (int)(millis / 1000 % 60));
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            break;
         case 0xca53: //task_info
            for(i = 0; i < NUMTHREADS; i++) {
               mysprintf(buf, "%d %s %n pr: %d %n sp: %x sb: %x su: %d %n sl: %d %n bl: %x",
               (int)tcbs[i].id, tcbs[i].name, 20-strlen(tcbs[i].name), (int)tcbs[i].priority, 4-ndigits(tcbs[i].priority), tcbs[i].stack_pointer, tcbs[i].stack_base, tcbs[i].stack_usage, 7-ndigits(tcbs[i].stack_usage), tcbs[i].sleep, 7-ndigits(tcbs[i].sleep), tcbs[i].block);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            }
            break;
         case 0x426e: //config_save
            config_save();
            break;
         case 0x987d: //address
            if(params_count(params)==2 && !params_integer(2,params)) {
               for(t=0, l=1,i=strlen((char*)params[2])-1; i>=2; l*= 16, i--)
                  t += l * (((char*)params[2])[i]>='0' && ((char*)params[2])[i]<='9' ? (((char*)params[2])[i]-'0') : (((char*)params[2])[i]>='a' && ((char*)params[2])[i]<='f' ? (10+((char*)params[2])[i]-'a') : (0)));
               if((t&3)==0 && ((t>=(unsigned int)&_flash_start && t<=(unsigned int)&_flash_end) || (t>=(unsigned int)&_ram_start && t<=(unsigned int)&_ram_end))) {
                  mysprintf(buf,"0x%x : 0x%x",t,*((unsigned int*)t));
                  output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               }
            }
            break;
         case 0x3f19: //output_mask
            if(params_count(params)==2) {
               for(i=0; i<(int)eOutputSubsystemLast; i++)
                  output_data.mask[i] = params[2];
            }
            else if(params_count(params)==3 && params[2]<eOutputSubsystemLast) {
               output_data.mask[params[2]] = params[3];
            }
            else {
               mysprintf(buf,"ADC %d",(int)eOutputSubsystemADC);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"BME280 %d",(int)eOutputSubsystemBME280);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"DS18B20 %d",(int)eOutputSubsystemDS18B20);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"Oled %d",(int)eOutputSubsystemOled);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"System %d",(int)eOutputSubsystemSystem);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"Switch %d",(int)eOutputSubsystemSwitch);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"None %d",(int)eOutputLevelNone);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"Debug %d",(int)eOutputLevelDebug);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"Normal %d",(int)eOutputLevelNormal);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               mysprintf(buf,"Important %d",(int)eOutputLevelImportant);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               for(i=0; i<(int)eOutputSubsystemLast; i++) {
                  mysprintf(buf, "[%d] %u",i,(unsigned int)output_data.mask[i]);
                  output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
               }
            }
            break;
         case 0xcb6e: //screen
            if(params_count(params)==1) {
               task_oled_data.screen = task_oled_data.screen%SCREENS+1;
            }
            else if(params_count(params)==2) {
               if(params[2]==0) {
                  task_oled_data.screen -= 1;
                  if(task_oled_data.screen==0) task_oled_data.screen = SCREENS;
               }
               else if(params[2]>=1 && params[2]<=SCREENS)
                  task_oled_data.screen = params[2];
            }
            break;
         case 0x7fb9: //units_p
            task_bme280_data.units_p ^= 1;
            break;
         case 0x5b34: //p_base
            task_bme280_data.base = 1;
            task_bme280_data.p_base = task_bme280_data.p;
            break;
         case 0xa577: //bme280_config
            mysprintf(buf, "dig_T1: %d", (int)bme280_data.dig_T1);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_T2: %d", (int)bme280_data.dig_T2);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_T3: %d", (int)bme280_data.dig_T3);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P1: %d", (int)bme280_data.dig_P1);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P2: %d", (int)bme280_data.dig_P2);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P3: %d", (int)bme280_data.dig_P3);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P4: %d", (int)bme280_data.dig_P4);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P5: %d", (int)bme280_data.dig_P5);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P6: %d", (int)bme280_data.dig_P6);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P7: %d", (int)bme280_data.dig_P7);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P8: %d", (int)bme280_data.dig_P8);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_P9: %d", (int)bme280_data.dig_P9);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_H1: %d", (int)bme280_data.dig_H1);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_H2: %d", (int)bme280_data.dig_H2);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_H3: %d", (int)bme280_data.dig_H3);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_H4: %d", (int)bme280_data.dig_H4);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_H5: %d", (int)bme280_data.dig_H5);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "dig_H6: %d", (int)bme280_data.dig_H6);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "t_fine: %d", (int)bme280_data.t_fine);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "osrs_h: %d", (int)bme280_data.osrs_h);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "osrs_p: %d", (int)bme280_data.osrs_p);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "osrs_t: %d", (int)bme280_data.osrs_t);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "uh: %d", (int)bme280_data.uh);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "up: %d", (int)bme280_data.up);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "ut: %d", (int)bme280_data.ut);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "ch: %d", (int)bme280_data.ch);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "cp: %d", (int)bme280_data.cp);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);

            mysprintf(buf, "ct: %d", (int)bme280_data.ct);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            break;
         case 0xed5: //psr
            mysprintf(buf, "0x%x", GetPSR());
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            break;
         case 0x7f7e: //iap_info
            t = iap_read_part_id();
            l = mysprintf(&buf[0], "Part id: 0x%x\r\n", t);
            t = iap_read_boot_code_version();
            l += mysprintf(&buf[l], "Boot code version: %d.%d\r\n", (t >> 8) & 0xff, t & 0xff);
            t = (unsigned int)iap_read_uid();
            l += mysprintf(&buf[l], "UID: %u %u %u %u", *((unsigned int *)t + 0), *((unsigned int *)t + 1), *((unsigned int *)t + 2), *((unsigned int *)t + 3));
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            break;
         case 0x3fab: //uart_in_enabled
            if(uart_data.uart_in_enabled) {
               ICER0 = (1<<3);
               uart_data.uart_in_enabled = 0;
            }
            else {
               ICPR0 = (1<<3);
               ISER0 = (1<<3);
               uart_data.uart_in_enabled = 1;
            }
            break;
         case 0x2b4a: //log_enabled
            task_oled_data.log_enabled ^= 1;
            break;
         case 0xfb7e: //led_enabled
            led_data.enabled ^= 1;
            if(!led_data.enabled) {
               led_data.counter = 0;
               LED_Off();
            }
            break;
         case 0xb400: //led_dc
            led_data.counter = 0;
            led_data.dc = params[2];
            break;
         case 0x4d2c: //led_period
            led_data.counter = 0;
            led_data.period = params[2];
            break;
         case 0xe89e: //crc16
            mysprintf(buf, "0x%x", (unsigned int)crc16((unsigned char *)params[2], strlen((char *)params[2])));
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant, 1);
            break;
         case 0xbf26: //temp
            break;
         default:
            output("Unknown command", eOutputSubsystemSystem, eOutputLevelImportant, 0);
      }
   }
}

void params_fill(char *s, unsigned int *params) {
   char *p,                     //pointer
     l,                         //length
     d,                         //all digits
     k;                         //# params
   int i;                       //iterator

   for(p = s, d = 1, k = 0, l = strlen(s), i = 0; i <= l; i++) {
      if(s[i] == ' ' || i == l) {
         s[i] = 0;
         params[k + 1] = d ? (params[0] |= (1 << (16 + k)), atoi(p)) : (unsigned int)p;
         k += 1;
         d = 1;
         p = &s[i + 1];
      }
      else {
         d = d && isdigit(s[i]);
      }
   }
   params[0] |= (k & 0xff);
}

int params_count(unsigned int *params) { //kiek parametru uzpildyta po params_fill ivykdymo
   return params[0] & 0xff;
}

int params_integer(char k, unsigned int *params) { //ar paremetras #k yra integer'is, jei ne -- jis yra pointeris i stringa
   return ((params[0] >> 16) & (1 << k)) != 0;
}
