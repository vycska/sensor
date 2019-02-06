#ifndef __TASK_OLED_H__
#define __TASK_OLED_H__

#include "u8g2.h"

#define I2C_SW 0
#define I2C_HW 1
#define SPI_SW 2
#define SPI_HW 3
#define OLED_COMMUNICATION SPI_SW

#define SCREENS 6

struct Task_Oled_Data {
   unsigned char log_buffer[32*10];
   volatile int screen, log_enabled;
};

void Task_Oled(void);

uint8_t u8x8_byte_hw_i2c(u8x8_t*,uint8_t,uint8_t,void*);
uint8_t u8x8_gpio_and_delay(u8x8_t*,uint8_t,uint8_t,void*);

#endif
