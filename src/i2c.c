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
   IPR2 = (IPR2&(~(3<<6))) | (1<<6); //I2C0 interrupt priority 1 (0 = highest, 3 = lowest)
   ISER0 = (1<<8); //I2C0 interrupt enable
   //set MSTEN in CFG
   I2C0CFG = (1<<0 | 0<<1 | 0<<2 | 0<<3 | 0<<4); //master enable, slave disable, monitor disabled, timeout disabled, monitor clock stretching disabled
}

int I2C0_Write(unsigned char slave,unsigned char *s,int k) {
   if((I2C0STAT&(1<<0))==1 && ((I2C0STAT>>1)&0x7)==0) { //master function is idle
   //write the slave address with RW set to 0 to MSTDAT
   //start the transmission by setting the MSTSTART
   
   while((I2C0STAT&(1<<0))==0); //wait for pending status to be set by polling STAT register
   if(((I2C0STAT>>1)&0x7)==2) { //transmit ready (ack received by slave)

   }
   //write 8 bits of data to the MSTDAT
   //continue with the transmission by setting MSTCONT
   //wait for pending status to be set
   //stop the transmission by setting MSTSTOP
   }
}

int I2C0_Read(unsigned char slave,unsigned char *s,int k) {
   //write the slave address with RW set to 1 to the MSTDAT
   //start the transmission by setting MSTSTART
   //wait for the pending status to be set
   //read 8 bits of data from MSTDAT
   //stop the transmission by setting MSTSTOP
}

void I2C_Handler(int k, struct I2C *i2c) {
   switch (k == 0 ? (I2C0STAT & 0xf8) : (k == 1 ? (I2C1STAT & 0xf8) : (I2C2STAT & 0xf8))) {
      case 0xf8:{
         break;
      }
         /* ***** general master states ***** */
      case 0x08:{              //START has been transmitted
         if(k == 0) {
            I2C0DAT = (unsigned char)((i2c->slave << 1) | ((i2c->direction == 1 ? 1 : 0) << 0));        //write slave address and direction [0 - write, 1 - read, 2 - write+read]
            I2C0CONCLR = (1 << 5);      //clear START
         }
         else if(k == 1) {
            I2C1DAT = (unsigned char)((i2c->slave << 1) | ((i2c->direction == 1 ? 1 : 0) << 0));
            I2C1CONCLR = (1 << 5);      //clear START
         }
         else {
            I2C2DAT = (unsigned char)((i2c->slave << 1) | ((i2c->direction == 1 ? 1 : 0) << 0));
            I2C2CONCLR = (1 << 5);      //clear START
         }
         break;
      }
      case 0x10:{              //repeated START has been transmitted
         if(k == 0) {
            I2C0DAT = (i2c->slave << 1) | (1 << 0);     //write slave address and direction 1 [read]
            I2C0CONCLR = (1 << 5);      //clear START
         }
         else if(k == 1) {
            I2C1DAT = (i2c->slave << 1) | (1 << 0);
            I2C1CONCLR = (1 << 5);      //clear START
         }
         else {
            I2C2DAT = (i2c->slave << 1) | (1 << 0);
            I2C2CONCLR = (1 << 5);      //clear START
         }
         break;
      }
         /* ***** master transmitter states ***** */
      case 0x18:{              //SLA+W has been transmitted and ACK received
         if(i2c->current < i2c->length[i2c->active_buffer]) {
            if(k == 0)
               I2C0DAT = i2c->buffer[i2c->active_buffer][i2c->current];
            else if(k == 1)
               I2C1DAT = i2c->buffer[i2c->active_buffer][i2c->current];
            else
               I2C2DAT = i2c->buffer[i2c->active_buffer][i2c->current];
            i2c->current += 1;
         }
         else {
            if(k == 0)
               I2C0CONSET = (1 << 4);   //set STOP
            else if(k == 1)
               I2C1CONSET = (1 << 4);
            else
               I2C2CONSET = (1 << 4);
            i2c->status = 1;    //i received ACK -- everything ok
         }
         break;
      }
      case 0x20:{              //slave address+write transmitted, NACK received
         if(k == 0)
            I2C0CONSET = (1 << 4);      //set STOP
         else if(k == 1)
            I2C1CONSET = (1 << 4);
         else
            I2C2CONSET = (1 << 4);
         i2c->status = -1;      //not what i expected
         break;
      }
      case 0x28:{              //data byte transmitted and ACK received
         if(i2c->current < i2c->length[i2c->active_buffer]) {   //dabartiniame buferyje yra neissiustu duomenu
            if(k == 0)
               I2C0DAT = i2c->buffer[i2c->active_buffer][i2c->current];
            else if(k == 1)
               I2C1DAT = i2c->buffer[i2c->active_buffer][i2c->current];
            else
               I2C2DAT = i2c->buffer[i2c->active_buffer][i2c->current];
            i2c->current += 1;
         }
         else if(++i2c->active_buffer == 1 && i2c->length[1] > 0) {     //dabartinis buferis isnaudotas, ziuriu sekanti
            if(i2c->direction == 2) {   //write baigtas, toliau seks read
               if(k == 0)
                  I2C0CONSET = (1 << 5);        //set START
               else if(k == 1)
                  I2C1CONSET = (1 << 5);
               else
                  I2C2CONSET = (1 << 5);
               i2c->current = 0;
            }
            else {              //issiunciamas sekancio buferio pirmas baitas
               if(k == 0)
                  I2C0DAT = i2c->buffer[1][0];
               else if(k == 1)
                  I2C1DAT = i2c->buffer[1][0];
               else
                  I2C2DAT = i2c->buffer[1][0];
               i2c->current = 1;
            }
         }
         else {                 //sekancio buferio nera arba jis tuscias
            if(k == 0)
               I2C0CONSET = (1 << 4);   //set STOP
            else if(k == 1)
               I2C1CONSET = (1 << 4);
            else if(k == 2)
               I2C2CONSET = (1 << 4);
            i2c->status = 1;    //viska issiunciau ka norejau
         }
         break;
      }
      case 0x30:{              //data transmitted, NACK received
         if(k == 0)
            I2C0CONSET = (1 << 4);      //set STOP
         else if(k == 1)
            I2C1CONSET = (1 << 4);
         else
            I2C2CONSET = (1 << 4);
         i2c->status = (i2c->current == i2c->length[i2c->active_buffer] ? 1 : -1);      //ziurau ar visus baitus issiunciau
         break;
      }
         /* ***** master receiver states ***** */
      case 0x40:{              //SLA+R has been transmitted and ACK received
         if(i2c->length[i2c->active_buffer] == 0) {     //neprasoma jokiu duomenu gavimo
            if(k == 0)
               I2C0CONSET = (1 << 4);   //set STOP
            else if(k == 1)
               I2C1CONSET = (1 << 4);
            else
               I2C2CONSET = (1 << 4);
            i2c->status = 1;
         }
         else if(i2c->length[i2c->active_buffer] == 1) {        //sekanti karta bus gautas vienintelis ir paskutinis baitas
            if(k == 0)
               I2C0CONCLR = (1 << 2);   //clear AA
            else if(k == 1)
               I2C1CONCLR = (1 << 2);
            else
               I2C2CONCLR = (1 << 2);
         }
         else {                 //bus gauta daugiau nei vienas baitas, todel is pradziu siusime AA
            if(k == 0)
               I2C0CONSET = (1 << 2);   //set AA
            else if(k == 1)
               I2C1CONSET = (1 << 2);
            else
               I2C2CONSET = (1 << 2);
         }
         break;
      }
      case 0x48:{              //slave address+read transmitted, NACK received
         if(k == 0)
            I2C0CONSET = (1 << 4);      //set STOP
         else if(k == 1)
            I2C1CONSET = (1 << 4);
         else
            I2C2CONSET = (1 << 4);
         i2c->status = -1;      //gal as ir nenorejau nieko nuskaityti, bet kadangi gavau NACK, laikau kad tai neigiamas dalykas
         break;
      }
      case 0x50:{              //data received, ACK returned
         i2c->buffer[i2c->active_buffer][i2c->current] = (k == 0 ? I2C0DAT : (k == 1 ? I2C1DAT : I2C2DAT));
         i2c->current += 1;
         if(i2c->current == i2c->length[i2c->active_buffer] - 1) {      //sekanti karta bus gautas paskutinis baitas
            if(k == 0)
               I2C0CONCLR = (1 << 2);   //clear AA
            else if(k == 1)
               I2C1CONCLR = (1 << 2);
            else
               I2C2CONCLR = (1 << 2);
         }
         else {
            if(k == 0)
               I2C0CONSET = (1 << 2);   //set AA
            else if(k == 1)
               I2C1CONSET = (1 << 2);
            else
               I2C2CONSET = (1 << 2);
         }
         break;
      }
      case 0x58:{              //data received, NACK returned [by me]
         i2c->buffer[i2c->active_buffer][i2c->current] = (k == 0 ? I2C0DAT : (k == 1 ? I2C1DAT : I2C2DAT));
         if(k == 0)
            I2C0CONSET = (1 << 4);      //set STOP
         else if(k == 1)
            I2C1CONSET = (1 << 4);
         else
            I2C2CONSET = (1 << 4);
         i2c->status = 1;
         break;
      }
      default:{
         if(k == 0) {
            I2C0CONCLR = (1 << 2);      //clear AA
            I2C0CONSET = (1 << 4);      //set STOP
         }
         else if(k == 1) {
            I2C1CONCLR = (1 << 2);
            I2C1CONSET = (1 << 4);
         }
         else {
            I2C2CONCLR = (1 << 2);
            I2C2CONSET = (1 << 4);
         }
         i2c->status = -2;
         break;
      }
   }
   if(k == 0)
      I2C0CONCLR = (1 << 3);    //clear SI
   else if(k == 1)
      I2C1CONCLR = (1 << 3);
   else
      I2C2CONCLR = (1 << 3);
}

void I2C_Start(int k) {
   if(k == 0) {
      ICPR0 |= (1 << 10);       //clear pending for I2C0
      I2C0CONCLR = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);   //clear AA, SI, STOP, STA
      I2C0CONSET = (1 << 5);    //set START
   }
   else if(k == 1) {
      ICPR0 |= (1 << 11);       //clear pending for I2C1
      I2C1CONCLR = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);   //clear AA, SI, STOP, STA
      I2C1CONSET = (1 << 5);    //set START
   }
   else {
      ICPR0 |= (1 << 12);       //clear pending for I2C2
      I2C2CONCLR = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);   //clear AA, SI, STOP, STA
      I2C2CONSET = (1 << 5);    //set START
   }
}

void I2C0_IRQHandler(void) {
   I2C_Handler(0, &i2c[0]);
}
