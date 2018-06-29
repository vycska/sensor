#include "os.h"
#include "main.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

int __attribute__((section (".stacks"))) stacks[1072]; //(576+576+576+576+576+576+576+256) / 4 = 4288 / 4
struct tcb tcbs[NUMTHREADS], *RunPt;

void OS_Init(int k, ...) {
   int i, stacks_offset;
   va_list va;

   va_start(va, k);
   for(stacks_offset = i = 0; i < k; i++) {
      tcbs[i].id = i;
      tcbs[i].sleep = 0;                                                   // thread initially not sleeeping
      tcbs[i].block = 0;                                                   // thread initially not blocked
      tcbs[i].name = va_arg(va, char *);
      tcbs[i].priority = va_arg(va, int);
      tcbs[i].stack_size = va_arg(va, int);
      stacks_offset += tcbs[i].stack_size/4;
      stacks[stacks_offset - 1] = 0x01000000;                              // PSR [thumb bit]
      stacks[stacks_offset - 2] = (int)va_arg(va, void (*)(void));         // PC
      stacks[stacks_offset - 3] = 0x14141414;                              // LR (R14)
      stacks[stacks_offset - 4] = 0x12121212;                              // R12
      stacks[stacks_offset - 5] = 0x03030303;                              // R3
      stacks[stacks_offset - 6] = 0x02020202;                              // R2
      stacks[stacks_offset - 7] = 0x01010101;                              // R1
      stacks[stacks_offset - 8] = 0x00000000;                              // R0
      stacks[stacks_offset - 9] = 0x11111111;                              // R11
      stacks[stacks_offset - 10] = 0x10101010;                             // R10
      stacks[stacks_offset - 11] = 0x09090909;                             // R9
      stacks[stacks_offset - 12] = 0x08080808;                             // R8
      stacks[stacks_offset - 13] = 0x07070707;                             // R7
      stacks[stacks_offset - 14] = 0x06060606;                             // R6
      stacks[stacks_offset - 15] = 0x05050505;                             // R5
      stacks[stacks_offset - 16] = 0x04040404;                             // R4
      tcbs[i].stack_maxusage = 0;
      tcbs[i].stack_base = (int)&stacks[stacks_offset];
      tcbs[i].stack_pointer = &stacks[stacks_offset - 16];                 // thread stack pointer
      tcbs[i].next = &tcbs[i + 1 != k ? (i + 1) : 0];                      // 0 points to 1, 1 points to 2, and so on
   }
   va_end(va);

   RunPt = &tcbs[0];
}

void OS_Scheduler(void) {
   int i, best_priority;
   struct tcb *cursor, *best_task;

   for(best_task = RunPt, best_priority = 128, cursor = RunPt->next, i = 0; i < NUMTHREADS; cursor = cursor->next, i++)
      if(cursor->priority < best_priority && cursor->block == 0 && cursor->sleep == 0) {
         best_priority = cursor->priority;
         best_task = cursor;
      }
   RunPt = best_task;
}

void Semaphore_Init(int *s, int v) {
   *s = v;
}

void Task_Suspend(void) {
   SYST_CVR = 0;
   ICSR |= (1 << 26);           // SysTick exception set pending
}

void Task_Sleep(int ms) {
   RunPt->sleep = ms;
   Task_Suspend();
}

void Task_Unsleep(struct tcb *task) {
   task->sleep = 0;
}

void Task_Suspend_Wait(int *s) {
   DisableInterrupts();
   while(*s <= 0) {
      EnableInterrupts();
      Task_Suspend();
      DisableInterrupts();
   }
   *s -= 1;
   EnableInterrupts();
}

void Task_Spinning_Wait(int *s) { //spinlock semaphore wait
   DisableInterrupts();
   while(*s <= 0) {
      EnableInterrupts();
      DisableInterrupts();
   }
   *s -= 1;
   EnableInterrupts();
}

void Task_Spinning_Signal(int *s) {
   DisableInterrupts();
   *s += 1;
   EnableInterrupts();
}

void Task_Blocking_Wait(int *s) { //blocking semaphore wait
   DisableInterrupts();
   *s -= 1;
   if(*s < 0) {                 //no pass available, thus block
      RunPt->block = s;
      EnableInterrupts();
      Task_Suspend();
   }
   EnableInterrupts();
}

void Task_Blocking_Signal(int *s) {
   int i;
   struct tcb *cursor;

   DisableInterrupts();
   *s += 1;
   if(*s <= 0) {                //wake up one thread
      for(cursor = RunPt->next, i = 0; i < NUMTHREADS && cursor->block != s; i += 1, cursor = cursor->next);
      if(i < NUMTHREADS)
         cursor->block = 0;
   }
   EnableInterrupts();
}

void SystemReset(void) {
   _DSB();
   AIRCR = (AIRCR & (~(1<<1 | 1<<2 | 0xffffu<<16))) | (1<<2 | 0x5fau<<16);
   _DSB();
   while(1);
}
