#include "main.h"
 
#define PIN0 33
#define PIN1 26
#define PIN2 27


void app_main(void)
{

   gpio_pad_select_gpio(PIN0);
   gpio_pad_select_gpio(PIN1);
   gpio_pad_select_gpio(PIN2);
   gpio_set_direction(PIN0, GPIO_MODE_OUTPUT);
   gpio_set_direction(PIN1, GPIO_MODE_OUTPUT);
   gpio_set_direction(PIN2, GPIO_MODE_OUTPUT);

  while (true){
    gpio_set_level(PIN0, 1);
    gpio_set_level(PIN1, 1);
    gpio_set_level(PIN2, 1);
  }

}

