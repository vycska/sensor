#include "task_command_parser.h"
#include "fifos.h"
#include "onewire.h"
#include "os.h"
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

extern volatile long long int millis;
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

      switch (crc16((unsigned char *)params[1], strlen((char *)params[1]))) {
         case 0xea72: { //owreset
            onewire_reset();
            break;
         }
         case 0x7646: { //owlow
            onewire_drivelinelow();
            break;
         }
         case 0xed1a: { //owhigh
            onewire_releaseline();
            break;
         }
         case 0x107e: { //owvalue
            unsigned char v;
            v = onewire_getlinevalue();
            mysprintf(buf,"%d",(int)v);
            Fifo_Uart0_Put(buf,&smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xeac9: { //owdelay
            onewire_drivelinelow();
            onewire_delay(500);
            onewire_releaseline();
            break;
         }
         case 0x178a: {         //system_reset
            SystemReset();
            break;
         }
         case 0x57e6: {         //millis
            mysprintf(buf, "%l", (char *)&millis);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xd89c: {         //live_time
            mysprintf(buf, "%d,%d:%d:%d", (int)(millis / 1000 / 60 / 60 / 24), (int)(millis / 1000 / 60 / 60 % 24), (int)(millis / 1000 / 60 % 60), (int)(millis / 1000 % 60));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xca53: {         //task_info
            for(i = 0; i < NUMTHREADS; i++) {
               mysprintf(buf, "%d %s %n pr: %d %n sp: %x sl: %d %n bl: %x",
               (int)tcbs[i].id, tcbs[i].name, 20-strlen(tcbs[i].name), (int)tcbs[i].priority, 4-ndigits(tcbs[i].priority), tcbs[i].sp, tcbs[i].sleep, 7-ndigits(tcbs[i].sleep), tcbs[i].block);
               Fifo_Uart0_Put(buf, &smphrFinished);
               OS_Blocking_Wait(&smphrFinished);
            }
            break;
         }
         case 0xed5: {          //psr
            mysprintf(buf, "0x%x", GetPSR());
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xe89e: {         //crc16
            mysprintf(buf, "0x%x", (unsigned int)crc16((unsigned char *)params[2], strlen((char *)params[2])));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
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
      if(s[i] == ':' || i == l) {
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
