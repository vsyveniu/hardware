#include "main.h"
#define EN_AMP 5
#define BUTT 	GPIO_NUM_18
#define LED_1 	GPIO_NUM_27


xQueueHandle interruptQueue;

static void IRAM_ATTR butt_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue, &pin, NULL);
}

void butt_pushed()
{
	uint32_t pin;
	int butt_1_on = 0;
	int count;
    int beep_count = 0;
    int volt = 0;
	while (true)
	{
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
	
			butt_1_on = !butt_1_on;
			
			gpio_set_level(LED_1, butt_1_on);

			gpio_intr_disable(pin);

			count = 0;

			while(gpio_get_level(pin) != 1 && count > 0){
				vTaskDelay(30 / portTICK_PERIOD_MS);
				if(gpio_get_level(pin) == 0)
					count++;
				else
					vTaskDelay(20 / portTICK_PERIOD_MS);
			}


			gpio_intr_enable(pin);

            for (beep_count = 0; beep_count < 1000; beep_count++){ 
            /* 
                    volt+=22;

                    dac_output_voltage(DAC_CHANNEL_1, volt);
                    volt-=12;
                    dac_output_voltage(DAC_CHANNEL_1, volt);
 */
              for (volt = 0; volt  < 256; volt+=100)
                { 
                    dac_output_voltage(DAC_CHANNEL_1, volt);
                
                }     
                ets_delay_us(1);
            }

                for (beep_count = 1000; beep_count > 0; beep_count--){ 

                /*  volt-=22;
                    dac_output_voltage(DAC_CHANNEL_1, volt);
                    volt+=12; */    
                    for (volt = 256; volt  > 0; volt-=8)
                    {
                        dac_output_voltage(DAC_CHANNEL_1, volt);
                    
                    }      
                ets_delay_us(1);
                }  

		}

	}
}



void app_main(void){

    gpio_config_t butt_conf = {
		.pin_bit_mask = GPIO_SEL_18,
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
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);

    xTaskCreate(butt_pushed, "button task", 2048, NULL, 1, NULL);
   
	 while (true){
         


	 }
}


