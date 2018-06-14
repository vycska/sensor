#include "task_idle.h"
#include "fifos.h"
#include "os.h"
#include "output.h"
#include "utils.h"
#include "utils-asm.h"

extern char _intvecs_size, _text_size, _rodata_size, _data_size, _bss_size, _stack_size, _heap_size;

void Task_Idle(void) {
   char s[32];

   output("Task_Idle has started", eOutputSubsystemSystem, eOutputLevelNormal, 0);

   mysprintf(s, "_intvecs_size: %u", (unsigned int)&_intvecs_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   mysprintf(s, "_text_size: %u", (unsigned int)&_text_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   mysprintf(s, "_rodata_size: %u", (unsigned int)&_rodata_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   mysprintf(s, "_data_size: %u", (unsigned int)&_data_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   mysprintf(s, "_bss_size: %u", (unsigned int)&_bss_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   mysprintf(s, "_stack_size: %u", (unsigned int)&_stack_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   mysprintf(s, "_heap_size: %u", (unsigned int)&_heap_size);
   output(s, eOutputSubsystemSystem, eOutputLevelNormal, 1);

   while(1) {
      WaitForInterrupt();
   }
}
