#include "main.h"
  
#define LED_1 	GPIO_NUM_27
#define LED_2 	GPIO_NUM_26
#define BUTT_1 	GPIO_NUM_39
#define BUTT_2 	GPIO_NUM_18
#define GPIO_BUTT1_BIT_MASK (1ULL<<GPIO_NUM_39)
#define GPIO_BUTT2_BIT_MASK (1ULL<<GPIO_NUM_18)
 
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
	//double butt_sec = 0.1;

	while (true)
	{
		//ESP_LOGI("butt_sec", "%f", butt_sec);

		//double time_sec = 0;
		//uint64_t time_val = 0;
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			//butt_state = 1;

			//  ESP_LOGI("level", "%d", gpio_get_level(pin));
			
			

			// //timer_get_counter_time_sec(TIMER_GROUP_0, 0, &time_sec);
		
			// 	/* code */
			// timer_start(TIMER_GROUP_0, TIMER_0);
			// timer_get_counter_value(TIMER_GROUP_0, 0, &time_val);
			// timer_get_counter_time_sec(TIMER_GROUP_0, 0, &time_sec);

			// if(gpio_get_level(pin) == 1){
				
					
			// 	butt_sec = time_sec;
			// 	//ESP_LOGI("asd", "%d", butt_sec);
			// 	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, time_val);
			// 	timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);

			// }
			
			// if(gpio_get_level(pin) == 0){
			
			// }
			// else{
				
			// }
			
			// 		printf("%" PRIu64 "\n", time_val);
			// ESP_LOGI("timer", "%f", time_sec);
					

			// }while(gpio_get_level(pin) != 0 && time_val >= 100);
					
			// while(gpio_get_level(pin) = 0){
			// 	vTaskDelay(20 / portTICK_PERIOD_MS);
			// 	if(gpio_get_level(pin) == 0)
			// 		count++;
			// 	else
			// 		vTaskDelay(20 / portTICK_PERIOD_MS);
			// }

			// do{
			// 	vTaskDelay(20 / portTICK_PERIOD_MS);
			// }while(gpio_get_level(pin) != 1);

			// ESP_LOGI("level----", "%d", gpio_get_level(pin));
			// printf("%s\n", "--------------");
			// if(gpio_get_level(pin) != 0){
			// 	ESP_LOGI("level", "%d", gpio_get_level(pin));
			// 	count = 0;
				//vTaskDelay(20 / portTICK_PERIOD_MS);
				// while(gpio_get_level(pin) != 0 && count > 1){
				// 	vTaskDelay(20 / portTICK_PERIOD_MS);
				// 	if(gpio_get_level(pin) == 0){
				// 		count++;

				// 	}
				// 	else
				// 		vTaskDelay(20 / portTICK_PERIOD_MS);
				// }

				// timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, time_val);
				// timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);
				//if(time_val > 100)
			count = 0;
			while(gpio_get_level(pin) != 1 && count > 1){
				vTaskDelay(20 / portTICK_PERIOD_MS);
				if(gpio_get_level(pin) == 0)
					count++;
				else
					vTaskDelay(20 / portTICK_PERIOD_MS);
			}

					butt_1_on = !butt_1_on;


				gpio_set_level(LED_1, butt_1_on);
				// timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, time_val);
				// timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);
			//}
		}

			//butt_state = 0;
		
	

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

			count = 0;
			while(gpio_get_level(pin) != 1 && count > 1){
				vTaskDelay(20 / portTICK_PERIOD_MS);
				if(gpio_get_level(pin) == 0)
					count++;
				else
					vTaskDelay(20 / portTICK_PERIOD_MS);
			}

			butt_2_on = !butt_2_on;


			gpio_set_level(LED_2, butt_2_on);
			
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
		.alarm_en = TIMER_ALARM_EN,
		.counter_en = false,
		.counter_dir = TIMER_COUNT_UP,
		.auto_reload = true,
		.divider = 16,
		.intr_type = TIMER_INTR_LEVEL,

	};




	// uint64_t time_val;
	// uint64_t alarm_val;
	// double time_sec;
	timer_init(TIMER_GROUP_0, TIMER_0, &timer_conf);
	

	// //timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

	// //timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 8);
	// //timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);

	// esp_err_t err = timer_isr_register(TIMER_GROUP_0, TIMER_0, butt_handler, (void *)BUTT_1, ESP_INTR_FLAG_IRAM, NULL);
	// //timer_group_intr_enable(TIMER_GROUP_0, TIMG_T0_INT_ENA_M);
	// //timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	//  if(err != ESP_OK){
	//  	abort();
	//  }s

	// //timer_start(TIMER_GROUP_0, 0);

	// // if(err != ESP_OK){
	// // 	abort();
	// // }

	// ESP_LOGI("timer", "%d", timer_get_counter_value(TIMER_GROUP_0, 0, &time_val));

	// while(true){
	// 	//timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &time_val);
	// 	timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &time_sec);
	// 	timer_get_alarm_value(TIMER_GROUP_0, TIMER_0, &alarm_val);

	// 	printf("%" PRIu64 "\n", alarm_val);
	// 	//ESP_LOGI("timer value", "PRIu64" , time_val);
	// 	ESP_LOGI("timer sec", "%f", time_sec);
		
	// }

	

	//timer_queue = xQueueCreate(10, sizeof(timer_event_t));
	//xTaskCreate(butt_handler, "timer_evt_task", 2048, NULL, 5, NULL);


	 gpio_install_isr_service(0);


	gpio_isr_handler_add(BUTT_1, butt1_handler, (void *)BUTT_1);
	gpio_isr_handler_add(BUTT_2, butt2_handler, (void *)BUTT_2);

	
	interruptQueue = xQueueCreate(10, sizeof(int));
	interruptQueue2 = xQueueCreate(10, sizeof(int));

	gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);



	xTaskCreate(butt1_pushed, "button 1 task", 2048, NULL, 1, NULL);
	xTaskCreate(butt2_pushed, "button 2 task", 2048, NULL, 1, NULL);

	while (true){

	}
}


