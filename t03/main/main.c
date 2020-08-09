#include "main.h"
  
#define LED_1 	GPIO_NUM_27
#define LED_2 	GPIO_NUM_26
#define BUTT_1 	GPIO_NUM_39
#define BUTT_2 	GPIO_NUM_18
#define GPIO_BUTT1_BIT_MASK (1ULL<<GPIO_NUM_39)
#define GPIO_BUTT2_BIT_MASK (1ULL<<GPIO_NUM_18)
 
xQueueHandle interruptQueue;
xQueueHandle interruptQueue2;

typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    uint64_t timer_counter_value;
} timer_event_t;

xQueueHandle timer_queue;
// static void IRAM_ATTR butt_handler(void *args){

// 	uint32_t pin = (uint32_t) args;
// 	xQueueSendFromISR(interruptQueue, &pin, NULL);
// }

static void IRAM_ATTR butt_handler(void *args){

	//uint32_t pin = (uint32_t) args;
	ESP_LOGI("interrpt", "%s", "fuck");
	//xQueueSendFromISR(interruptQueue, &pin, NULL);
}

//  static void IRAM_ATTR butt2_handler(void *args){

// 	uint32_t pin = (uint32_t) args;
// 	xQueueSendFromISR(interruptQueue2, &pin, NULL);
// }  


void butt1_pushed()
{
	uint32_t pin;
	bool butt_1_on = false;

	while (true)
	{
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			butt_1_on = !butt_1_on;

			do
			{
				vTaskDelay(20 / portTICK_PERIOD_MS);
			} while(gpio_get_level(pin) == 0);
			

			if(butt_1_on)
				gpio_set_level(LED_1, 1);
			else
				gpio_set_level(LED_1, 0);
		}
	}
}

void butt2_pushed()
{
	uint32_t pin;
	bool butt_2_on = false;

	while (true)
	{
		if(xQueueReceive(interruptQueue2, &pin, portMAX_DELAY))
		{
			butt_2_on = !butt_2_on;

			do
			{
				vTaskDelay(20 / portTICK_PERIOD_MS);
			} while(gpio_get_level(pin) == 0);

			gpio_set_level(LED_2, 1);

			if(butt_2_on)
				gpio_set_level(LED_2, 1);
			else
				gpio_set_level(LED_2, 0);

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

	timer_config_t timer_conf= {
		.alarm_en = false,
		.counter_en = true,
		.counter_dir = TIMER_COUNT_UP,
		.auto_reload = true,
		.divider = 16,
	};

	timer_group_intr_enable(TIMER_GROUP_0, TIMG_T0_INT_ENA_M);
	timer_isr_register(TIMER_GROUP_0, 0, butt_handler, (void *)BUTT_1, ESP_INTR_FLAG_IRAM, NULL);

	timer_set_alarm_value(TIMER_GROUP_0, 0, 5000);
	timer_set_alarm(TIMER_GROUP_0, 0, TIMER_ALARM_EN);
	timer_init(TIMER_GROUP_0, 0, &timer_conf);

	timer_queue = xQueueCreate(10, sizeof(timer_event_t));
	xTaskCreate(butt_handler, "timer_evt_task", 2048, NULL, 5, NULL);
	// gpio_install_isr_service(0);


	// gpio_isr_handler_add(BUTT_1, butt_handler, (void *)BUTT_1);
	// gpio_isr_handler_add(BUTT_2, butt2_handler, (void *)BUTT_2);

	
	// interruptQueue = xQueueCreate(10, sizeof(int));
	// interruptQueue2 = xQueueCreate(10, sizeof(int));

	// gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
	// gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);



	//xTaskCreate(butt1_pushed, "button 1 task", 2048, NULL, 1, NULL);
	//xTaskCreate(butt2_pushed, "button 2 task", 2048, NULL, 1, NULL);

	// while (true){

	// }
}


