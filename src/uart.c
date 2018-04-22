#include "uart.h"
#include "fifos.h"
#include "main.h"
#include "os.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct sUART_In uart0_in;

//baudrate: 9600, main_clock: 60000000, uartdiv: 250, divided_clock: 240000, mult: 144, u_pclk: 153600, brgval: 1
void UART0_Init(void) {
   USART0CFG &= (~(1<<0)); //disable usart0
   IPR0 = (IPR0&(~(3u<<30))) | (2u<<30); //UART0 interrupt priority 2 (0 = highest, 3 = lowest)
   ISER0 = (1<<3); //UART0 interrupt enable
   PINENABLE0 |= (1<<0 | 1<<24); //disable ACMP_I1 on PIO0_0, disable ADC_11 on PIO0_4
   PINASSIGN0 = (PINASSIGN0&(~(0xff<<0 | 0xff<<8))) | (4<<0 | 0<<8); //U0TXD assigned to PIO0_4, U0RXD assigned to PIO0_0
   UARTCLKDIV = 250; //configure the clock divider for the fractional baud rate generator
   UARTFRGDIV = 0xff; //denominator of the fractional divider
   UARTFRGMULT = 144; //numerator of the fractional divider
   USART0BRG = 0; //this value+1 is used to divide clock to determine the baud rate
   USART0CTL = (0<<1 | 0<<2 | 0<<6 | 0<<16); //no break, no address detect mode, transmit not disabled, autobaud disabled
   USART0OSR= 0xf; //oversample 16
   USART0CFG = (1<<0 | 1<<2 | 0<<4 | 0<<6 | 0<<9 | 0<<11 | 0<<15); //USART0 enable, 8b data length, no parity, 1 stop bit, no flow control, asynchronous mode, no loopback mode
   USART0INTENSET = (1<<0); //interrupt when there is a received character
}

void UART0_Transmit(char *s, int k) {
   int i;
   for(i=0;i<k+2;i++) { //+2 nes gale pridesim \r\n
      while((USART0STAT&(1<<2))==0); //wait until TXRDY
      USART0TXDAT = (i == k ? '\r' : (i == k + 1 ? '\n' : s[i]));
   }
}

void UART0_IRQHandler(void) {
   unsigned char c;

   if(USART0INTSTAT&(1<<0)) { //RXRDY
      c = USART0RXDAT&0xff;
      if(isprint(c)) {
         uart0_in.s[uart0_in.i] = c;
         uart0_in.i = (uart0_in.i + 1) % UART_IN_MAX;
      }
      else if(uart0_in.i != 0) {
         //send received command to the FIFO buffer
         uart0_in.s[uart0_in.i] = 0;
         Fifo_Command_Parser_Put(uart0_in.s);
         uart0_in.i = 0;
      }
   }
}
