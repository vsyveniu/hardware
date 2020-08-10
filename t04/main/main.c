#include "main.h"
  
#define LED_1 	GPIO_NUM_27
#define LED_2 	GPIO_NUM_26
#define DHT 	GPIO_NUM_4
#define BUTT_2 	GPIO_NUM_18
#define GPIO_BUTT1_BIT_MASK (1ULL<<GPIO_NUM_39)
#define GPIO_BUTT2_BIT_MASK (1ULL<<GPIO_NUM_18)
 





void app_main(void)
{
	gpio_config_t butt_1_conf = {
		.pin_bit_mask = GPIO_SEL_4,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};
	gpio_config_t butt_2_conf = {
		.pin_bit_mask = GPIO_SEL_18,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};


	gpio_config(&butt_1_conf);
	gpio_config(&butt_2_conf);




	 while (true){
	 	ESP_LOGI("dht", "%d", gpio_get_level(DHT));
	 }
}


