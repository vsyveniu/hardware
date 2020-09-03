#include "main.h"

#define EN_AMP  5
#define BUTT 	GPIO_NUM_39

xQueueHandle interruptQueue;

static void IRAM_ATTR butt_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue, &pin, NULL);
}

void butt_pushed()
{
	uint32_t pin;
	int count;
    int beep_count = 0;
    int volt = 0;
	while (true)
	{
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
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

            for (beep_count = 0; beep_count < 1000; beep_count++){
				for (volt = 0; volt  < 256; volt+=100)
				{ 
                    dac_output_voltage(DAC_CHANNEL_1, volt);
				}     
                ets_delay_us(1);
            }

			for (beep_count = 1000; beep_count > 0; beep_count--){ 
				for (volt = 256; volt  > 0; volt-=8)
                {
                    dac_output_voltage(DAC_CHANNEL_1, volt); 
                }      
                ets_delay_us(1);
            }

			gpio_intr_enable(pin);  
		}
	}
}

void app_main(void){

    gpio_config_t butt_conf = {
		.pin_bit_mask = GPIO_SEL_39,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config(&butt_conf);
    
    gpio_set_direction(EN_AMP, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_AMP, 1);
    dac_output_enable(DAC_CHANNEL_1);

    gpio_install_isr_service(0);

	gpio_isr_handler_add(BUTT, butt_handler, (void *)BUTT);

    interruptQueue = xQueueCreate(10, sizeof(int));

    xTaskCreate(butt_pushed, "button task", 2048, NULL, 1, NULL);

}


