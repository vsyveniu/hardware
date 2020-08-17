#include "main.h"
#define EN_AMP 5
#define EN_OLED GPIO_NUM_32




void app_main(void){

	gpio_set_direction(EN_OLED, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_OLED, 1);
   
	 while (true){
         


	 }
}


