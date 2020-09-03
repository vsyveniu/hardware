#include "main.h"
  
#define LED_1 	GPIO_NUM_27
#define LED_2 	GPIO_NUM_26
#define LED_3 	GPIO_NUM_33
#define BUTT_1 	GPIO_NUM_39
#define BUTT_2 	GPIO_NUM_18
 
xQueueHandle interruptQueue;
xQueueHandle interruptQueue2;

static void IRAM_ATTR butt1_handler(void *args){

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
	int butt_1_on = 0;
	int count;

	while (true)
	{
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			butt_1_on = !butt_1_on;
			gpio_set_level(LED_1, butt_1_on);
			gpio_intr_disable(pin);

			count = 0;

			while(gpio_get_level(pin) != 1 && count < 10){
				if(gpio_get_level(pin) == 0){
					vTaskDelay(10/portTICK_PERIOD_MS);
					count = 0;
				}
				else if(gpio_get_level(pin) == 1){
					count++;
				}				
			}
			gpio_intr_enable(pin);
		}

	}
}
void butt2_pushed()
{
	uint32_t pin;
	int butt_2_on = 0;
	int count;

	while (true)
	{
		if(xQueueReceive(interruptQueue2, &pin, portMAX_DELAY))
		{
			butt_2_on = !butt_2_on;
			gpio_set_level(LED_2, butt_2_on);
			gpio_intr_disable(pin);

			count = 0;

			while(gpio_get_level(pin) != 1 && count < 10){
				if(gpio_get_level(pin) == 0){
					vTaskDelay(10/portTICK_PERIOD_MS);
					count = 0;
				}
				else if(gpio_get_level(pin) == 1){
					count++;
				}				
			}
			gpio_intr_enable(pin);		
		}
	}
}


void app_main(void)
{
	gpio_config_t butt_1_conf = {
		.pin_bit_mask = GPIO_SEL_39,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};
	gpio_config_t butt_2_conf = {
		.pin_bit_mask = GPIO_SEL_18,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config(&butt_1_conf);
	gpio_config(&butt_2_conf);

	gpio_install_isr_service(0);

	gpio_isr_handler_add(BUTT_1, butt1_handler, (void *)BUTT_1);
	gpio_isr_handler_add(BUTT_2, butt2_handler, (void *)BUTT_2);

	interruptQueue = xQueueCreate(10, sizeof(int));
	interruptQueue2 = xQueueCreate(10, sizeof(int));

	gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);

	xTaskCreate(butt1_pushed, "button 1 task", 2048, NULL, 1, NULL);
	xTaskCreate(butt2_pushed, "button 2 task", 2048, NULL, 1, NULL);

}


