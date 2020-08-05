#include "main.h"
 
#define PIN_OUT_0 33
#define PIN_OUT_1 26
#define PIN_OUT_2 27


void app_main(void)
{

  gpio_set_direction(PIN_OUT_0, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_OUT_1, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_OUT_2, GPIO_MODE_OUTPUT);

  while (true){
    gpio_set_level(PIN_OUT_0, 1);
    gpio_set_level(PIN_OUT_1, 1);
    gpio_set_level(PIN_OUT_2, 1);
  }

}

