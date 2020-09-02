void butt1_pushed()
{
	uint32_t pin;
	int count;

	while (true){
		if(xQueueReceive(button1Queue, &pin, portMAX_DELAY))
		{

			gpio_intr_disable(pin);

			oled_page = OLED_PAGE_TEMPERATURE;
			beep();

			count = 0;

			while(count != 1){
				if(count > 1){
					count = 0;
				}
				if(gpio_get_level(pin) == 0){
					count++;
				}
				else if(gpio_get_level(pin) == 1){
					count++;
					vTaskDelay(15/portTICK_PERIOD_MS);
					if(gpio_get_level(pin) == 1)
						count = 1;
				}
					
			}
			gpio_intr_enable(pin);
			
		}
	}
}

void butt2_pushed()
{
	uint32_t pin;
	int count;

	while (true)
	{
		
		if(xQueueReceive(button2Queue, &pin, portMAX_DELAY))
		{
			gpio_intr_disable(pin);

			oled_page = OLED_PAGE_HUMIDITY;
			beep();
			count = 0;

			while(count != 1){
				if(count > 1){
					count = 0;
				}
				if(gpio_get_level(pin) == 0){
					count++;
				}
				else if(gpio_get_level(pin) == 1){
					count++;
					vTaskDelay(15/portTICK_PERIOD_MS);
					if(gpio_get_level(pin) == 1)
						count = 1;
				}
					
			}
			gpio_intr_enable(pin);
			
		}
	}
}