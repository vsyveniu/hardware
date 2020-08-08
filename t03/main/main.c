#include "main.h"
  
#define LED_1 	GPIO_NUM_27
#define LED_2 	GPIO_NUM_26
#define BUTT_1 	GPIO_NUM_39
#define BUTT_2 	GPIO_NUM_18
#define GPIO_BUTT1_BIT_MASK (1ULL<<GPIO_NUM_39)
#define GPIO_BUTT2_BIT_MASK (1ULL<<GPIO_NUM_18)
 
xQueueHandle interruptQueue;
xQueueHandle interruptQueue2;

static void IRAM_ATTR butt_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue, &pin, NULL);
}

 static void IRAM_ATTR butt2_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue2, &pin, NULL);
}  



void butt1_pushed()
{
	
	uint32_t pin;
	while (true)
	{
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			ESP_LOGI("push", "%d button pushed %d ", pin, gpio_get_level(pin));
			ESP_LOGI("push", "%s", "rttr");
			gpio_set_level(LED_1, 1);
		}
	}
}

void butt2_pushed()
{
	
	uint32_t pin;
	while (true)
	{
		if(xQueueReceive(interruptQueue2, &pin, portMAX_DELAY))
		{
			ESP_LOGI("push", "%d button pushed %d ", pin, gpio_get_level(pin));
			gpio_set_level(LED_2, 1);

		}
	}
}

void app_main(void)
{
	gpio_config_t butt_1_conf = {
		.pin_bit_mask = GPIO_BUTT1_BIT_MASK,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};
	gpio_config_t butt_2_conf = {
		.pin_bit_mask = GPIO_BUTT2_BIT_MASK,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};


	gpio_config(&butt_1_conf);
	gpio_config(&butt_2_conf);

	gpio_install_isr_service(0);


	gpio_isr_handler_add(BUTT_1, butt_handler, (void *)BUTT_1);
	gpio_isr_handler_add(BUTT_2, butt2_handler, (void *)BUTT_2);

	
	interruptQueue = xQueueCreate(10, sizeof(int));
	interruptQueue2 = xQueueCreate(10, sizeof(int));

	 gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
	 gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);

	xTaskCreate(butt1_pushed, "button 1 task", 2048, NULL, 1, NULL);
	xTaskCreate(butt2_pushed, "button 2 task", 2048, NULL, 1, NULL);
	//xTaskCreate(butt2_pushed);

	while (true){
		//ESP_LOGI("level-1", "%d", gpio_get_level(BUTT_1));

		//ESP_LOGI("level-2", "%d", gpio_get_level(BUTT_2));
	}
}


