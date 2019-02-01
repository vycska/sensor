$ JLinkGDBServer -device cortex-m0+ -endian little -if SWD -speed 4000 -port 4444

$ arm-none-eabi-gdb sensor.elf

(gdb) target remote localhost:4444
Remote debugging using localhost:4444

(gdb) load
Loading section .intvecs, size 0xc0 lma 0x0
Loading section .text, size 0x5dd0 lma 0xc0
Loading section .rodata, size 0x1930 lma 0x5e90
Loading section .ARM.exidx.text.__divdi3, size 0x8 lma 0x77c0
Loading section .data, size 0x218 lma 0x77c8
Start address 0x13c, load size 31200
Transfer rate: 165 KB/sec, 5200 bytes/write.

(gdb) monitor reset
Resetting target

(gdb) step
init () ar src/main.c:137
137   for(src = &_data_start_lma, dst = &_data_start; dst < &_data_end; src++, dst++)

(gdb) info program
Debugging a target over a serial line.
Program stopped at 0x118c.
It stopped after being stepped.
Type "info stack" or "info registers" for more information.

(gdb) info stack
#0 init () at src/main.c:137
#1 0x00000140 in Reset_Handler () at startup.s:64
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

(gdb) info registers
r0          0x0
r1          0x13d
r2          0x0
r3          0x0
r4          0x40048000
r5          0xeffffa35
r6          0x12345678
r7          0xffffffff
r8          0xffffffff
r9          0xffffffff
r10         0xffffffff
r11         0xffffffff
r12         0xffffffff
sp          0x10000200  0x10000200 <stacks>
lr          0x141
pc          0x118c      0x118c <init>
xpsr        0x61000000
msp         0x10000200
psp         0xfffffffc
primask     0x0
basepri     0x0
faultmask   0x0
control     0x0

(gdb) info locals
dst = <optimized out>
src = <optimized out>

(gdb) info frame
Stack level 0, frame at 0x10000200:
 pc = 0x118c in init (src/main.c:137); saved pc = 0x140
 called by frame at 0x10000200
 source language c.
 Arglist at 0x10000200, args:
 Locals at 0x10000200, Previous frame's sp is 0x10000200

(gdb) print/x $sp
0x10000200

(gdb) p/x $lr
0x141

(gdb) list
void init(void) {
   char *dst, *src;
   //copy data to ram
   for(src=&_data_start_lma, dst=&_data_start; dst < &_data_end; src++, dst++)
     *dst = *src;
   //zero bss
   for(dst=&_bss_start; dst < &_bss_end; dst++)
      *dst = 0;

(gdb) finish
Run till exit from #0 init () at src/main.c:137
Reset_Handler () at startup.s:65
65 bl main

(gdb) step
main () at src/main.c:41
41 t = config_load();

(gdb) info frame
Stack level 0, frame at 0x10000200:
 pc = 0xe54 in main (src/main.c:41); saved pc = 0x144
 source language c.
 Arglist at 0x10000150, args:
 Saved registers: r4 at 0x100001ec, r5 at 0x100001f0, r6 at 0x100001f4, r7 at 0x100001f8, lr at 0x100001fc

(gdb) info line *0x144
Line 66 of "startup.s" starts at address 0x144 <Reset_Handler+8> and ends at 0x146 <WKT_IRQHandler>

(gdb) x/64xw 0x10000150
0x10000150: 0x000029f1  0x00006068  0x00000003  0x00000240
0x10000160: 0x00002abd  0x00006058  0x00000004  0x00000240
0x10000170: 0x00001c3d  0x00006050  0x00000006  0x00000240
0x10000180: 0x000019e9  0x00006048  0x00000006  0x00000240
0x10000190: 0x00002571  0x00006040  0x00000007  0x00000240
0x100001a0: 0x00002669  0x00006034  0x00000008  0x00000240
0x100001b0: 0x000020fd  0x0000602c  0x0000001f  0x00000100
0x100001c0: 0x00002661  0xbda69728  0x206d6172  0x64657375
0x100001d0: 0x3138203a  0x39003434  0x5b000032  0x34383632
0x100001e0: 0x34363334  0x5d005d38  0x70594b00  0x40048000
0x100001f0: 0xeffffa35  0x12345678  0xffffffff  0x00000145

(gdb) print &buf
(char (*)[32]) 0x100001c8

(gdb) print buf
"ram used: 8144\000\071\062\000\000[268443648]\000]"

========================================================

$ JLinkGDBServer -device cortex-m0+ -endian little -if SWD -speed 4000 -port 4444
SEGGER J-Link GDB Server V6.32h Command Line Version
JLinkARM.dll V6.32h (DLL compiled Jul  5 2018 18:14:58)
Command line: -device cortex-m0+ -endian little -if SWD -speed 4000 -port 4444
-----GDB Server start settings-----
GDBInit file:                  none
GDB Server Listening port:     4444
SWO raw output listening port: 2332
Terminal I/O port:             2333
Accept remote connection:      yes
Generate logfile:              off
Verify download:               off
Init regs on start:            off
Silent mode:                   off
Single run mode:               off
Target connection timeout:     0 ms
------J-Link related settings------
J-Link Host interface:         USB
J-Link script:                 none
J-Link settings file:          none
------Target related settings------
Target device:                 cortex-m0+
Target interface:              SWD
Target interface speed:        4000kHz
Target endian:                 little
Connecting to J-Link...
J-Link is connected.
Firmware: J-Link EDU Mini V1 compiled Mar 29 2018 17:48:40
Hardware: V1.00
S/N: 801003831
Feature(s): GDB, FlashBP
Checking target voltage...
Target voltage: 3.26 V
Listening on TCP/IP port 4444
Connecting to target...Connected to target
Waiting for GDB connection...

$ arm-none-eabi-gdb --quiet --readnow sensor.elf
Reading symbols from sensor.elf...expanding to full symbols...done.

(gdb) show version
GNU gdb (GDB) 8.1
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=arm-none-eabi".

(gdb) target remote localhost:4444
Remote debugging using localhost:4444

(gdb) load
Loading section .intvecs, size 0xc0 lma 0x0
Loading section .text, size 0x5cd0 lma 0xc0
Loading section .rodata, size 0x1910 lma 0x5d90
Loading section .ARM.exidx.text.__divdi3, size 0x8 lma 0x76a0
Loading section .data, size 0x218 lma 0x76a8
Start address 0x13c, load size 30912
Transfer rate: 70 KB/sec, 5152 bytes/write.

(gdb) info float
No floating-point info available for this processor.

(gdb) show directories
Source directories searched: $cdir:$cwd

(gdb) show endian
The target endianness is set automatically (currently little endian)

(gdb) show history
expansion: History expansion on command input is off.
filename: The filename in which to record the command history is "/home/vytas/Programming/Embedded/sensor/.gdb_history".
remove-duplicates: The number of history entries to look back at for duplicates is 0.
save: Saving of the history record on exit is off.
size: The size of the command history is 256.

(gdb) info target
(gdb) info files
Symbols from "/home/vytas/Programming/Embedded/sensor/sensor.elf".
Remote serial target in gdb-specific protocol:
Debugging a target over a serial line.
   While running this, GDB does not access memory from...
Local exec file:
   '/home/vytas/Programming/Embedded/sensor/sensor.elf',
   file type elf32-littlearm.
   Entry point: 0x13c
   0x00000000 - 0x000000c0 is .intvecs
   0x000000c0 - 0x00005d90 is .text
   0x00005d90 - 0x000076a0 is .rodata
   0x000076a0 - 0x000076a8 is .ARM.exidx.text.__divdi3
   0x10000000 - 0x10000200 is .stack
   0x10000200 - 0x10000fc0 is .stacks
   0x10000fc0 - 0x100011d8 is .data
   0x100011d8 - 0x10001bd0 is .bss
   0x10001bd0 - 0x10002000 is .heap

(gdb) info source
Current source file is startup.s
Compilation directory is /home/vytas/Programming/Embedded/sensor
Located in /home/vytas/Programming/Embedded/sensor/startup.s
Contains 138 lines.
Source language is asm.
Producer is GNU AS 2.30.
Compiled with DWARF 2 debugging format.
Does not include preprocessor macro info.

(gdb) info threads
   Id Target Id      Frame
*  1  Thread 57005   Reset_Handler () ar startup.s:64

(gdb) info program
Debugging a target over a serial line.
Program stopped at 0x11ac.
It stopped with signal SIGTRAP, Trace/breakpoint trap.

(gdb) info stack
#0 Reset_Handler () at startup.s:64
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

(gdb) frame
#0 Reset_Handler () at startup.s:64
64 bl init

(gdb) backtrace
#0 Reset_Handler () at startup.s:64
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

(gdb) info frame
Stack level 0, frame at 0x10000200:
   pc = 0x13c in Reset_Handler (startup.s:64);
      saved pc = <not saved>
   Outermost frame: previous frame identical to this frame (corrupt stack?)
   source language asm.
   Arglist at 0x10000200, args:
   Locals at 0x10000200, Previous frame's sp is 0x10000200

(gdb) info locals
No locals.

(gdb) show listsize
Number or source lines gdb will list by default is 10.

(gdb) list
59
60 .text
61 .thumb_func
62 .type Reset_Handler, %function
63 Reset_Handler:
64 bl init
65 bl main
66 b .
67
68 .thumb_func

(gdb) list 63,65
63 Reset_Handler:
64 bl init
65 bl main

(gdb) info line 64
Line 64 of "startup.s" starts at address 0x13c <Reset_Handler> and ends at 0x140 <Reset_Handler+4>.

(gdb) disassemble 0x13c
Dump of assembler code for function Reset_Handler:
=> 0x0000013c  <+0>: bl    0x1184 <init>
   0x00000140  <+4>: bl    0xe50 <main>
   0x00000144  <+8>: b.n   0x144 <Reset_Handler+8>

(gdb) x/i 0x13c
=> 0x13c <Reset_Handler>:     bl 0x1184 <init>

(gdb)
   0x140 <Reset_Handler+4>:   bl 0xe50 <main>

(gdb) x/2wx 0x13c
0x13c <Reset_Handler>:  0xf822f001  0xfe86f000

(gdb) info registers
r0          0x100011d8
r1          0x0
r2          0x10001bd0
r3          0x100011dd
r4          0x40048000
r5          0xeffffa35
r6          0x12345678
r7          0xffffffff
r8          0xffffffff
r9          0xffffffff
r10         0xffffffff
r11         0xffffffff
r12         0xffffffff
sp          0x10000200  <stacks>
lr          0x141
pc          0x13c       <Reset_Handler>
xpsr        0x81000000
msp         0x10000200
psp         0xfffffffc
primask     0x0
basepri     0x0
faultmask   0x0
control     0x0

(gdb) p/x *0x13c@3
$1 = {0xf822f001, 0xfe86f000, 0xe7fee7fe}

(gdb) set print symbol-filename on

(gdb) print/a 0xe60
$2 = 0xe60  <main+16 at src/main.c:42>

(gdb) x/i $pc
=> 0x13c <Reset_Handler at startup.s:64>:
   bl 0x1184   <init at src/main.c:134>

(gdb) show language
The current source language is "auto; currently asm".

(gdb) info address millis
Symbol "millis" is static storage at address 0x10001850.

(gdb) whatis millis
type = volatile long long

(gdb) ptype switch_data
type = volatile struct Switch_Data {
   int active;
   int delay;
   int duration;
   long long start;
}

(gdb) info sources

(gdb) info types

(gdb) info variables

(gdb) info functions

(gdb) show commands
   74 info source
   75 info sources
   76 info sources
   77 info functions
   78 info variables
   79 print millis
   80 info target
   81 show endian
   82 show history
   83 show commands

(gdb) f
#0 Reset_Hanler () at startup.s:64
64 bl init

(gdb) step
init () at src/main.c:134
for(src = &_data_start_lma, dst = &_data_start; dst < &_data_end; src++, dst++)

(gdb) info frame
Stack level 0, frame at 0x10000200:
   pc = 0x1184 in init (src/main.c:134); saved pc = 0x140
   called by frame at 0x10000200
   source language c.
   Arglist at 0x10000200, args:
   Locals at 0x10000200, Previous frame's sp is 0x10000200

(gdb) info registers pc lr
pc    0x1184   <init at src/main.c:134>
lr    0x141

(gdb) info line 134
Line 134 of "src/main.c"
   starts at address 0x1184 <init at src/main.c:134>
   and ends at 0x1192 <init+14 at src/main.c:135>.

(gdb) set print symbol-filename off

(gdb) x/2i 0x1184
=> 0x1184 <init>:
   ldr r2,  [pc, #48]   ;  (0x11b8 <init+52>)
   0x1186 <init+2>:
   ldr r3, [pc, #52]    ;  (0x11bc <init+56>)

(gdb) x/2wx 0x11b8
0x11b8 <init+52>:    0x10000fc0  0x100011d8

(gdb) finish
Run till exit from #0 init () at src/main.c:134
Reset_Handler () at startup.s:65
65 bl main

(gdb) p/x $pc
$3 = 0x140

(gdb) s
main () at src/main.c"40
40 t = config_load();

(gdb) info frame
Stack level 0, frame at 0x10000200:
   pc = 0xe54 in main (src/main.c:40); saved pc = 0x144
   source language c.
   Arglist at 0x10000150, args:
   Locals at 0x10000150, Previous frame's sp is 0x10000200
   Saved registers:
   r4 at 0x100001ec, r5 at 0x100001f0, r6 at 0x100001f4, r7 at 0x100001f8, lr at 0x100001fc

(gdb) p/x $sp
$4 = 0x10000150

(gdb) p/d 0x10000200-0x10000150
$5 = 176

(gdb) p 176/4
$6 = 44

(gdb) x/44wx 0x10000150
0x10000150: 0x000029e9  0x00005f68  0x00000003  0x00000100
0x10000160: 0x00002ab5  0x00005f58  0x00000004  0x00000240
0x10000170: 0x00001c35  0x00005f50  0x00000006  0x00000240
0x10000180: 0x000019e1  0x00005f48  0x00000006  0x00000240
0x10000190: 0x00002569  0x00005f40  0x00000007  0x00000240
0x100001a0: 0x00002661  0x00005f34  0x00000008  0x00000100
0x100001b0: 0x00002af5  0x00005f2c  0x0000001f  0x00000080
0x100001c0: 0x00002659  0xb586872c  0x7865745f  0x69735f74
0x100001d0: 0x203a657a  0x36373332  0x5b000030  0x34383632
0x100001e0: 0x34363334  0x5d005d38  0x78584900  0x40048000
0x100001f0: 0xeffffa35  0x12345678  0xffffffff  0x00000145

(gdb) next
42 PDRUNCFG &= (~(1<<0 | 1<<1 | 1<<2 | 1<<4 | 1<<7)); //IRC output, IRC, flash, ADC, PLL powered

(gdb) break SysTick_Handler
Breakpoint 1 at 0xfe: file src/os-asm.s, line 54.

(gdb) commands
>printf "millis: "
>print/d millis
>end

(gdb) continue
Continuing.
Breakpoint 1, SysTick_Handler () at src/os-asm.s:54
54 cpsid i
millis:  $7 = 0

(gdb) commands 1
>printf "millis: %d\n",millis
>end

(gdb) c
Continuing.
Breakpoint 1, SysTick_Handler () at src/os-asm.s:54
54 cpsid i
millis: 1

(gdb) disable 1

(gdb) info breakpoints
Num   Type        Disp     Enb   Address     What
1     breakpoint  keep     n     0x000000fe  src/os-asm.s:54
      breakpoint already hit 25 times
      printf "millis: %d\n", millis

(gdb) watch task_oled_data.log enabled
Hardware watchpoint 2: task_oled_data.log_enabled

(gdb) c
Continuing.
Hardware watchpoint 2: task_oled_data.log_enabled
Old value = 1
New value = 0
0x00001cf2 in Task_Command_Parser () at src/task_command_parser.c:44
44 Fifo_Command_Parser_Get(&pString);

(gdb) 
