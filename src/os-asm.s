.syntax unified
.cpu cortex-m0plus
.thumb

.text

.global OS_Start
.global SysTick_Handler

.thumb_func
OS_Start:
ldr r0,=RunPt
ldr r1,[r0]
ldr r2,[r1]
msr msp,r2
isb

pop {r4-r7}

pop {r0-r3}
mov r8,r0
mov r9,r1
mov r10,r2
mov r11,r3

pop {r0-r3}

msr psp,r0
pop {r0}
mov r12,r0

add sp,#4
isb

pop {r0}
mov lr,r0

add sp,#4
isb

push {lr}
ldr r0,=10
bl SysTick_Init
bl MRT0_Init

pop {r0}
mov lr,r0

mrs r0,psp

cpsie i
bx lr

.thumb_func
SysTick_Handler:
cpsid i

push {r4-r7}
mov r4,r8
mov r5,r9
mov r6,r10
mov r7,r11
push {r4-r7}

ldr r0,=RunPt
ldr r1,[r0]
mrs r2,msp
str r2,[r1]

push {r0,lr}
bl Scheduler

pop {r0,r1}
mov lr,r1

ldr r1,[r0]
ldr r2,[r1]
msr msp,r2
isb

pop {r4-r7}

pop {r0-r3}
mov r8,r0
mov r9,r1
mov r10,r2
mov r11,r3

cpsie i
bx lr

.end
