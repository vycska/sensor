#ifndef __UART_H__
#define __UART_H__

#define UART_IN_MAX 48

void UART0_Init(void);
void UART0_Transmit(char*);

struct UART_Data {
   char s[UART_IN_MAX];
   int i,uart_in_enabled;
};

#endif
