#include "i2c.h"
#include "main.h"
#include "os.h"
#include "utils.h"
#include "lpc824.h"
#include <stdlib.h>

void I2C0_Init(void) {
   PINENABLE0 &= (~(1<<11 | 1<<12)); //I2C0_SDA enabled on pin PIO0_11, I2C0_SCL enabled on pin PIO0_10
   PIO0_11 = (0<<6 | 0<<8 | 0<<11 | 0<<13); //input not inverted, standard/fast mode I2C, bypass input filter, peripheral clock divider 0
   PIO0_10 = (0<<6 | 0<<8 | 0<<11 | 0<<13); //input not inverted, standard/fast mode I2C, bypass input filter, peripheral clock divider 0
   I2C0CLKDIV = (CLOCK-1); //pclk divider to produce function clock of 1MHz
   I2C0MSTTIME = (3<<0 | 3<<4); //SCL low time is 3+2, SCL high time is 3+2
   I2C0TIMEOUT |= (0xfff<<4); //timeout value (will not be used)
   I2C0INTENCLR = (1<<0 | 1<<4 | 1<<6 | 1<<8 | 1<<11 | 1<<15 | 1<<16 | 1<<17 | 1<<19 | 1<<24 | 1<<25); //no interrupts
   //IPR2 = (IPR2&(~(3<<6))) | (1<<6); //I2C0 interrupt priority 1 (0 = highest, 3 = lowest)
   //ISER0 = (1<<8); //I2C0 interrupt enable
   I2C0CFG = (1<<0 | 0<<1 | 0<<2 | 0<<3 | 0<<4); //master enable, slave disable, monitor disabled, timeout disabled, monitor clock stretching disabled
}

int I2C0_Transaction(struct I2C *i2c) {
   int i,j,ok;

   //if((I2C0STAT&(1<<0))==1 && ((I2C0STAT>>1)&0x7)==0) { //master function is idle
      for(ok=1,i=0;i<=1 && ok;i++) { //2 buferiai
         for(j=0;j<i2c->length[i] && ok;j++) {
            while((I2C0STAT&(1<<0))==0); //wait for pending status to be set
            if((i==0 && j==0) || ((i==1 && j==0 && i2c->direction==2)?(i2c->direction=1):0)) {
               I2C0MSTDAT = ((i2c->slave<<1) | (i2c->direction==1)); //write the slave address with RW
               I2C0MSTCTL = (1<<1); //start the transmission by setting the MSTSTART
               while((I2C0STAT&(1<<0))==0); //wait for pending status to be set by polling STAT register
               ok = (i2c->direction==1)?(((I2C0STAT>>1)&0x7)==1):(((I2C0STAT>>1)&0x7)==2);
            }
            else ok = ((I2C0STAT>>1)&0x7)!=4; //slave did not NACKed transmitted data
            if(ok) {
               if(i2c->direction==1)
                  i2c->buffer[i][j] = I2C0MSTDAT;
               else //write 8 bits of data to the MSTDAT
                  I2C0MSTDAT = i2c->buffer[i][j];
               //!!! kuriais atvejais reikia siusti MSTCONT -- gal jei issiunti duomenis tai turi siusti continue 
               I2C0MSTCTL = (1<<0); //continue with the transmission by setting MSTCONT
            }
         }
      }
      while((I2C0STAT&(1<<0))==0); //wait for pending status to be set
      I2C0MSTCTL = (1<<2); //stop the transmission by setting MSTSTOP
   //}
   //else ok = -1;
   return ok;
}
