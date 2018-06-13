#include "task_command_parser.h"
#include "bme280.h"
#include "fifos.h"
#include "iap.h"
#include "led.h"
#include "onewire.h"
#include "os.h"
#include "switch.h"
#include "task_switch.h"
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

extern volatile struct Dump dump;

extern volatile long long int millis;
extern struct BME280_Config bme280_config;
extern struct LED_Data led_data;
extern struct tcb *RunPt;
extern struct tcb tcbs[NUMTHREADS];

void Task_Command_Parser(void) {
   char *pString, buf[128];
   int i, smphrFinished;
   unsigned int params[8];

   Fifo_Uart0_Put("Task_Command_Parser has started", 0);
   OS_InitSemaphore(&smphrFinished, 0);
   while(1) {
      Fifo_Command_Parser_Get(&pString);
      mysprintf(buf, "<< %s >>", pString);
      Fifo_Uart0_Put(buf, &smphrFinished);
      OS_Blocking_Wait(&smphrFinished);

      memset(params, 0, sizeof(params));
      params_fill(pString, params);

      switch(crc16((unsigned char *)params[1], strlen((char *)params[1]))) {
         case 0x178a: { //system_reset
            SystemReset();
            break;
         }
         case 0x57e6: { //millis
            mysprintf(buf, "%l", (char *)&millis);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xd89c: { //live_time
            mysprintf(buf, "%d,%d:%d:%d", (int)(millis / 1000 / 60 / 60 / 24), (int)(millis / 1000 / 60 / 60 % 24), (int)(millis / 1000 / 60 % 60), (int)(millis / 1000 % 60));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xca53: { //task_info
            for(i = 0; i < NUMTHREADS; i++) {
               mysprintf(buf, "%d %s %n pr: %d %n sp: %x sl: %d %n bl: %x",
               (int)tcbs[i].id, tcbs[i].name, 20-strlen(tcbs[i].name), (int)tcbs[i].priority, 4-ndigits(tcbs[i].priority), tcbs[i].sp, tcbs[i].sleep, 7-ndigits(tcbs[i].sleep), tcbs[i].block);
               Fifo_Uart0_Put(buf, &smphrFinished);
               OS_Blocking_Wait(&smphrFinished);
            }
            break;
         }
         case 0x8b8b: { //switch
            break;
         }
         case 0xa577: { //bme280_config
            mysprintf(buf, "dig_T1: %d", (int)bme280_config.dig_T1);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_T2: %d", (int)bme280_config.dig_T2);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_T3: %d", (int)bme280_config.dig_T3);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P1: %d", (int)bme280_config.dig_P1);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P2: %d", (int)bme280_config.dig_P2);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P3: %d", (int)bme280_config.dig_P3);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P4: %d", (int)bme280_config.dig_P4);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P5: %d", (int)bme280_config.dig_P5);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P6: %d", (int)bme280_config.dig_P6);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P7: %d", (int)bme280_config.dig_P7);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P8: %d", (int)bme280_config.dig_P8);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P9: %d", (int)bme280_config.dig_P9);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H1: %d", (int)bme280_config.dig_H1);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H2: %d", (int)bme280_config.dig_H2);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H3: %d", (int)bme280_config.dig_H3);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H4: %d", (int)bme280_config.dig_H4);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H5: %d", (int)bme280_config.dig_H5);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H6: %d", (int)bme280_config.dig_H6);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "t_fine: %d", (int)bme280_config.t_fine);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "os_h: %d", (int)bme280_config.os_h);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "os_p: %d", (int)bme280_config.os_p);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "os_t: %d", (int)bme280_config.os_t);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "adc_H: %d", (int)bme280_config.adc_H);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "adc_P: %d", (int)bme280_config.adc_P);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "adc_T: %d", (int)bme280_config.adc_T);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "H: %d", (int)bme280_config.H);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "P: %d", (int)bme280_config.P);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "T: %d", (int)bme280_config.T);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xed5: { //psr
            mysprintf(buf, "0x%x", GetPSR());
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0x7f7e: { //iap_info
            int len;
            unsigned int result;

            result = iap_read_part_id();
            len = mysprintf(&buf[0], "Part id: 0x%x\r\n", result);
            result = iap_read_boot_code_version();
            len += mysprintf(&buf[len], "Boot code version: %d.%d\r\n", (result >> 8) & 0xff, result & 0xff);
            result = (unsigned int)iap_read_uid();
            len += mysprintf(&buf[len], "UID: %u %u %u %u", *((unsigned int *)result + 0), *((unsigned int *)result + 1), *((unsigned int *)result + 2), *((unsigned int *)result + 3));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xfb7e: { //led_enabled
            led_data.enabled ^= 1;
            if(!led_data.enabled) {
               led_data.counter = 0;
               LED_Off();
            }
            break;
         }
         case 0xb400: { //led_dc
            led_data.counter = 0;
            led_data.dc = params[2];
            break;
         }
         case 0x4d2c:{ //led_period
            led_data.counter = 0;
            led_data.period = params[2];
            break;
         }
         case 0xe89e: { //crc16
            mysprintf(buf, "0x%x", (unsigned int)crc16((unsigned char *)params[2], strlen((char *)params[2])));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xbf26: { //temp
            for(i=0;i<20;i++) {
               mysprintf(buf,"[%d]: %l",i,dump.millis[i]);
               Fifo_Uart0_Put(buf, &smphrFinished);
               OS_Blocking_Wait(&smphrFinished);
            }
            break;
         }
         default: {
            Fifo_Uart0_Put("Unknown command", 0);
         }
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
