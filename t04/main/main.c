#include "main.h"
#define DHT 	GPIO_NUM_4






void app_main(void)
{
	gpio_config_t butt_1_conf = {
		.pin_bit_mask = GPIO_SEL_4,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config(&butt_1_conf);


	int val;
	 while (true){

	 	adc2_get_raw(ADC2_CHANNEL_4, 8, &val);

	 	printf("%d\n", val);

	 }
}


