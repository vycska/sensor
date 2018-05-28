#ifndef __I2C_H__
#define __I2C_H__

struct I2C {
   unsigned char *buffer[2],
                 slave,
                 direction;
   int length[2];
};

void I2C0_Init(void);
int I2C0_Transaction(struct I2C*);

#endif
