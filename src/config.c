#include "config.h"
#include "iap.h"
#include "led.h"
#include "task_oled.h"
#include "lpc824.h"
#include <string.h>

extern volatile struct LED_Data led_data;
extern struct Task_Oled_Data task_oled_data;

struct Config_Data config_data[] = {
   {(char*)&task_oled_data.screen,     (short)sizeof(task_oled_data.screen)},
   {(char*)&led_data.enabled,          (short)sizeof(led_data.enabled)}
};

int config_load(void) {
   char *p = (char*)(CONFIG_PAGE*64);
   int i,o,total_size,result;
   for(total_size=0,i=0; i<sizeof(config_data)/sizeof(struct Config_Data); i++)
      total_size += config_data[i].size;
   if(total_size == *((int*)p)) { //config'as geras, sprendziant pagal dydi (tik)
      result = 0;
      for(o=4,i=0; i<sizeof(config_data)/sizeof(struct Config_Data); o+=config_data[i++].size)
         memcpy(config_data[i].address_in_ram, p+o, config_data[i].size);
   }
   else result = 1;
   return result;
}

void config_save(void) {
   char p[64];
   int i,o;
   memset(p,0xff,64);
   for(o=4,i=0; i<sizeof(config_data)/sizeof(struct Config_Data); o+=config_data[i++].size)
      memcpy(p+o, config_data[i].address_in_ram, config_data[i].size);
   *((int*)p) = o-4;
   iap_erase_page(CONFIG_PAGE, CONFIG_PAGE);
   iap_copy_ram_to_flash(CONFIG_PAGE*64, (unsigned char*)p, 64);
}
