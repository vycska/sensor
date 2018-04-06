#ifndef __LPC1769_H__
#define __LPC1769_H__

#include <stdint.h>

// I/O configuration
#define PIO0_17                        (*((volatile uint32_t*)0x40044000)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_13                        (*((volatile uint32_t*)0x40044004)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_12                        (*((volatile uint32_t*)0x40044008)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_5                         (*((volatile uint32_t*)0x4004400c)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_4                         (*((volatile uint32_t*)0x40044010)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_3                         (*((volatile uint32_t*)0x40044014)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_2                         (*((volatile uint32_t*)0x40044018)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_11                        (*((volatile uint32_t*)0x4004401c)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_10                        (*((volatile uint32_t*)0x40044020)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_16                        (*((volatile uint32_t*)0x40044024)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_15                        (*((volatile uint32_t*)0x40044028)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_1                         (*((volatile uint32_t*)0x4004402c)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_9                         (*((volatile uint32_t*)0x40044034)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_8                         (*((volatile uint32_t*)0x40044038)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_7                         (*((volatile uint32_t*)0x4004403c)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_6                         (*((volatile uint32_t*)0x40044040)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_0                         (*((volatile uint32_t*)0x40044044)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_14                        (*((volatile uint32_t*)0x40044048)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_28                        (*((volatile uint32_t*)0x40044050)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_27                        (*((volatile uint32_t*)0x40044054)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_26                        (*((volatile uint32_t*)0x40044058)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_25                        (*((volatile uint32_t*)0x4004405c)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_24                        (*((volatile uint32_t*)0x40044060)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_23                        (*((volatile uint32_t*)0x40044064)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_22                        (*((volatile uint32_t*)0x40044068)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_21                        (*((volatile uint32_t*)0x4004406c)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_20                        (*((volatile uint32_t*)0x40044070)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_19                        (*((volatile uint32_t*)0x40044074)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_18                        (*((volatile uint32_t*)0x40044078)) //I/O configuration for pin PIO0_17/ADC_9

// system configuration
#define SYSMEMREMAP                    (*((volatile uint32_t*)0x40048000)) //system memory remap
#define PRESETCTRL                     (*((volatile uint32_t*)0x40048004)) //peripheral reset control
#define SYSPLLCTRL                     (*((volatile uint32_t*)0x40048008)) //system pll control
#define SYSPLLSTAT                     (*((volatile uint32_t*)0x4004800c)) //system pll status
#define SYSOSCCTRL                     (*((volatile uint32_t*)0x40048020)) //system oscillator control
#define WDTOSCCTRL                     (*((volatile uint32_t*)0x40048024)) //watchdog oscillator control
#define IRCCTRL                        (*((volatile uint32_t*)0x40048028)) //irc control
#define SYSRSTSTAT                     (*((volatile uint32_t*)0x40048030)) //system reset status
#define SYSPLLCLKSEL                   (*((volatile uint32_t*)0x40048040)) //system pll clock source select
#define SYSPLLCLKUEN                   (*((volatile uint32_t*)0x40048044)) //system pll clock source update enable
#define MAINCLKSEL                     (*((volatile uint32_t*)0x40048070)) //main clock source select
#define MAINCLKUEN                     (*((volatile uint32_t*)0x40048074)) //main clock source update enable
#define SYSAHBCLKDIV                   (*((volatile uint32_t*)0x40048078)) //system clock divider
#define SYSAHBCLKCTRL                  (*((volatile uint32_t*)0x40048080)) //system clock control
#define UARTCLKDIV                     (*((volatile uint32_t*)0x40048094)) //USART clock divider
#define CLKOUTSEL                      (*((volatile uint32_t*)0x400480e0)) //CLKOUT clock source select
#define CLKOUTUEN                      (*((volatile uint32_t*)0x400480e4)) //CLKOUT clock source update enable
#define CLKOUTDIV                      (*((volatile uint32_t*)0x400480e8)) //CLKOUT clock divider
#define UARTFRGDIV                     (*((volatile uint32_t*)0x400480f0)) //USART1 to USART4 common fractional generator divider value
#define UARTFRGMULT                    (*((volatile uint32_t*)0x400480f4)) //USART1 to USART4 common fractional generator multiplier value
#define EXTTRACECMD                    (*((volatile uint32_t*)0x400480fc)) //external trace buffer command
#define PIOPORCAP0                     (*((volatile uint32_t*)0x40048100)) //POR captured PIO status 0
#define IOCONCLKDIV6                   (*((volatile uint32_t*)0x40048134)) //peripheral clock 6 to the IOCON block for programmable glitch
#define IOCONCLKDIV5                   (*((volatile uint32_t*)0x40048138)) //peripheral clock 5 to the IOCON block for programmable glitch
#define IOCONCLKDIV4                   (*((volatile uint32_t*)0x4004813c)) //peripheral clock 4 to the IOCON block for programmable glitch
#define IOCONCLKDIV3                   (*((volatile uint32_t*)0x40048140)) //peripheral clock 3 to the IOCON block for programmable glitch
#define IOCONCLKDIV2                   (*((volatile uint32_t*)0x40048144)) //peripheral clock 2 to the IOCON block for programmable glitch
#define IOCONCLKDIV1                   (*((volatile uint32_t*)0x40048148)) //peripheral clock 1 to the IOCON block for programmable glitch
#define IOCONCLKDIV0                   (*((volatile uint32_t*)0x4004814c)) //peripheral clock 0 to the IOCON block for programmable glitch
#define BODCTRL                        (*((volatile uint32_t*)0x40048150)) //brown-out detect
#define SYSTCKCAL                      (*((volatile uint32_t*)0x40048154)) //system tick counter calibration
#define IRQLATENCY                     (*((volatile uint32_t*)0x40048170)) //IRQ delay; allows trade-off between interrupt latency and
#define NMISRC                         (*((volatile uint32_t*)0x40048174)) //NMI source control
#define PINTSEL0                       (*((volatile uint32_t*)0x40048178)) //GPIO pin interrupt select register 0
#define PINTSEL1                       (*((volatile uint32_t*)0x4004817c)) //GPIO pin interrupt select register 1
#define PINTSEL2                       (*((volatile uint32_t*)0x40048180)) //GPIO pin interrupt select register 2
#define PINTSEL3                       (*((volatile uint32_t*)0x40048184)) //GPIO pin interrupt select register 3
#define PINTSEL4                       (*((volatile uint32_t*)0x40048188)) //GPIO pin interrupt select register 4
#define PINTSEL5                       (*((volatile uint32_t*)0x4004818c)) //GPIO pin interrupt select register 5
#define PINTSEL6                       (*((volatile uint32_t*)0x40048190)) //GPIO pin interrupt select register 6
#define PINTSEL7                       (*((volatile uint32_t*)0x40048194)) //GPIO pin interrupt select register 7
#define STARTERP0                      (*((volatile uint32_t*)0x40048204)) //start logic 0 pin wake-up enable
#define STARTERP1                      (*((volatile uint32_t*)0x40048214)) //start logic 1 interrupt wake-up enable
#define PDSLEEPCFG                     (*((volatile uint32_t*)0x40048230)) //power-down states in deep-sleep mode
#define PDAWAKECFG                     (*((volatile uint32_t*)0x40048234)) //power-down states for wake-up from deep-sleep
#define PDRUNCFG                       (*((volatile uint32_t*)0x40048238)) //power configuration register
#define DEVICE_ID                      (*((volatile uint32_t*)0x400483f8)) //device id

// switch matrix
#define PINASSIGN0                     (*((volatile uint32_t*)0x4000c000)) //pin assign register 0
#define PINASSIGN1                     (*((volatile uint32_t*)0x4000c004)) //pin assign register 1
#define PINASSIGN2                     (*((volatile uint32_t*)0x4000c008)) //pin assign register 2
#define PINASSIGN3                     (*((volatile uint32_t*)0x4000c00c)) //pin assign register 3
#define PINASSIGN4                     (*((volatile uint32_t*)0x4000c010)) //pin assign register 4
#define PINASSIGN5                     (*((volatile uint32_t*)0x4000c014)) //pin assign register 5
#define PINASSIGN6                     (*((volatile uint32_t*)0x4000c018)) //pin assign register 6
#define PINASSIGN7                     (*((volatile uint32_t*)0x4000c01c)) //pin assign register 7
#define PINASSIGN8                     (*((volatile uint32_t*)0x4000c020)) //pin assign register 8
#define PINASSIGN9                     (*((volatile uint32_t*)0x4000c024)) //pin assign register 9
#define PINASSIGN10                    (*((volatile uint32_t*)0x4000c028)) //pin assign register 10
#define PINASSIGN11                    (*((volatile uint32_t*)0x4000c02c)) //pin assign register 11
#define PINENABLE0                     (*((volatile uint32_t*)0x4000c1c0)) //pin enable register

// NVIC
#define ISER0                          (*((volatile uint32_t*)0xe000e100)) //interrupt set enable
#define ICER0                          (*((volatile uint32_t*)0xe000e180)) //interrupt clear enable
#define ISPR0                          (*((volatile uint32_t*)0xe000e200)) //interrupt set pending
#define ICPR0                          (*((volatile uint32_t*)0xe000e280)) //interrupt clear pending
#define IABR0                          (*((volatile uint32_t*)0xe000e300)) //interrupt active bit
#define IPR0                           (*((volatile uint32_t*)0xe000e400)) //interrupt priority registers 0
#define IPR1                           (*((volatile uint32_t*)0xe000e404)) //interrupt priority registers 1
#define IPR2                           (*((volatile uint32_t*)0xe000e408)) //interrupt priority registers 2
#define IPR3                           (*((volatile uint32_t*)0xe000e40c)) //interrupt priority registers 3
#define IPR4                           (*((volatile uint32_t*)0xe000e410)) //interrupt priority registers 4
#define IPR5                           (*((volatile uint32_t*)0xe000e414)) //interrupt priority registers 5
#define IPR6                           (*((volatile uint32_t*)0xe000e418)) //interrupt priority registers 6
#define IPR7                           (*((volatile uint32_t*)0xe000e41c)) //interrupt priority registers 7

#endif
