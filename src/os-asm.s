.syntax unified
.cpu cortex-m0plus
.thumb

.text

.global OS_Start
.global SysTick_Handler

.thumb_func
OS_Start:
@load stack pointer from RunPt
ldr r0,=RunPt
ldr r1,[r0]
ldr r2,[r1]
mov sp,r2

@pop r4-r7 and r8-r11 from stack
pop {r4-r7}
pop {r0-r3}
mov r8,r0
mov r9,r1
mov r10,r2
mov r11,r3

@initialize systick and timer for sleeping threads
ldr r0,=10
bl SysTick_Init
bl MRT0_Init

@special value in lr for restoring the rest of the registers
ldr r0,=0xfffffff9
mov lr,r0

cpsie i
bx lr

.thumb_func
SysTick_Handler:
cpsid i

@store r8-r11 and r4-r7 in the stack
mov r0,r8
mov r1,r9
mov r2,r10
mov r3,r11
push {r0-r3}
push {r4-r7}

@save stack pointer in the tcb
ldr r0,=RunPt
ldr r1,[r0]
mov r2,sp
str r2,[r1]

@run scheduler with argument RunPt in r0
push {r0,lr}
bl Scheduler

@restore r0 and lr
pop {r0,r1}
mov lr,r1

@assign new stack pointer value
ldr r1,[r0]
ldr r2,[r1]
mov sp,r2

@pop r4-r7 and r8-r11 from the new stack
pop {r4-r7}
pop {r0-r3}
mov r8,r0
mov r9,r1
mov r10,r2
mov r11,r3

cpsie i
bx lr

.end
