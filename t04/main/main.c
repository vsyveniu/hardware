#include "main.h"
#define DHT_PIN GPIO_NUM_4
#define DHT_POWER GPIO_NUM_2



void sendStart(){

	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_level(DHT_PIN, 0); 
	vTaskDelay(20 / portTICK_PERIOD_MS);
	gpio_set_level(DHT_PIN, 1);
	ets_delay_us(43);
	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);

	
}


int get_humidity(){

	sendStart();
	

	int count = 0;
	printf("%d%s\n", gpio_get_level(DHT_PIN), " ------ after mcu");

/* 	while (gpio_get_level(DHT_PIN)==1)
	{
		if(count > 40)
		{
				return ESP_ERR_TIMEOUT;
		}	
		count++;
		ets_delay_us(1);
	} */

	ets_delay_us(81);
	printf("%d%s\n", gpio_get_level(DHT_PIN), " ------ after dht");

	count = 0;

	/* while(gpio_get_level(DHT_PIN) == 0){
		if(gpio_get_level(DHT_PIN) == 1){
			printf("%s\n", "80 us low out    ");
			break;
		}
		if(count > 90){
			//printf("%d\n", gpio_get_level(DHT_PIN));
			printf("%s\n", "80 us timeout");
			//return ESP_ERR_TIMEOUT;
		}
		count++;
		//printf("%d\n", count);
		ets_delay_us(1);
	} */

	printf("%d", count);


	/* count = 0;
	ets_delay_us(100);
	while(count < 80 && gpio_get_level(DHT_PIN) == 1){
		//printf("%s", "fuck");
		
		if(gpio_get_level(DHT_PIN) == 0){
			printf("%s\n", "80 us high out");
			break;
		}
		if(count > 80 && gpio_get_level(DHT_PIN) == 1){
			printf("%s\n", "80 us timeout");
		}
		printf("%d%s\n", gpio_get_level(DHT_PIN), "   80 high");
		count++;
		ets_delay_us(1);
	} */

	//printf("%d\n", count);



		//count = 0;

/* 	while(count < 50){
		if(gpio_get_level(DHT_PIN) == 1){
			//printf("%s\n", "50 us low out");
		}
		count++;
		ets_delay_us(1);
	}
	while(count < 70){
		if(gpio_get_level(DHT_PIN) == 0){
			//printf("%s\n", "70 us high out");
		}
		count++;
		ets_delay_us(1);
	}
 */
	


	//printf("%d%s\n", gpio_get_level(DHT_PIN),  " after all");
	/* int j = 0;
	while(j < 40){

		int i = 0;
		while(gpio_get_level(DHT_PIN) != 1){
				if(gpio_get_level(DHT_PIN) == 1){
					printf("%s\n", "50us passed");
					break;
				} 
				if(i > 55){
					printf("%d\n", gpio_get_level(DHT_PIN));
					printf("%s\n", "timeout");
					break;
				}
				i++;
				printf("%d\n", i);
				ets_delay_us(1);
				
			}
			j++;
	}
	printf("%d\n", j); */
	/* count = 0;
	int i = 0;
	while(count < 40){
		while(gpio_get_level(DHT_PIN) == 0){
			 if(gpio_get_level(DHT_PIN) == 1){
				 printf("%s", "50us passed");
				 break;
			} 
			if(i > 50){
				printf("%s", "timeout");
				break;
			}
			i++;
			printf("%d\n", i);
			ets_delay_us(1);
		}
		i = 0;
		printf("%d%s\n", gpio_get_level(DHT_PIN), "  level");
		while(gpio_get_level(DHT_PIN) == 1){
			i++;
			printf("%d%s", i, "in level 1");
			ets_delay_us(1);
		}
		if(i == 28){
			printf("%d%s", i, "  0 bit 28us");
			bits[count] = 0;
		}
		else if( i == 70){
			printf("%d%s", i, "  1 bit 70us");
			bits[count] = 1;
		}

		count++;
	}


	int j = 0;
	while(j < 40){
		//printf("%d%s%d\n", bits[j], "  this is bit  ", j);
		j++;
	} */

	printf("%s\n", "-----------");
	return 0;
}

int get_temperature(){

	return 0;
}




void app_main(void)
{
	/*  gpio_config_t dht_conf= {
	 	.pin_bit_mask = GPIO_SEL_4,
		.mode = GPIO_MODE_OUTPUT,
	 	.pull_up_en = GPIO_PULLUP_ENABLE,
	 	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	};

	 gpio_config(&dht_conf); */

	gpio_set_direction(DHT_POWER, GPIO_MODE_OUTPUT);	
	gpio_set_level(DHT_POWER, 1);
	vTaskDelay(3000 / portTICK_PERIOD_MS);
		
	 while (true){

	 	//printf("%d\n", get_temperature());
	
		get_humidity();
	 	vTaskDelay(3000 / portTICK_PERIOD_MS);
	 	
		
		 //printf("%d%s\n", gpio_get_level(DHT_PIN),  "   wait inside send");
		 
	 	//printf("%d\n", get_humidity());

	 }
}


