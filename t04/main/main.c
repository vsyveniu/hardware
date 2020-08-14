#include "main.h"
#define DHT_PIN GPIO_NUM_4
#define DHT_POWER GPIO_NUM_2
#define HUM_BYTE_INDEX 0
#define TEMP_BYTE_INDEX 2



int get_all_data(){


  u_int8_t dataBytes[5];
  int count = 0;
  int byteNum = 0;
  int bitNum = 7;
  int bitCount = 0;

 

  for (int i = 0; i <5; i++)
  		dataBytes[i] = 0; 


  	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_level(DHT_PIN, 0); 
	vTaskDelay(20 / portTICK_PERIOD_MS);
	gpio_set_level(DHT_PIN, 1);

	count = 0;
	while(gpio_get_level(DHT_PIN) != 0){
		gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
		if(count > 45){
			printf("%s\n", "DHT11 is retarded");
			return ESP_ERR_TIMEOUT;
		}
		if(gpio_get_level(DHT_PIN) == 0){
			break;
		}
		count++;
		ets_delay_us(1);
	}
	count = 0;
	while(gpio_get_level(DHT_PIN) == 0)
	{
		if(count > 80)
		{
			printf("%s\n", "DHT11 is retarded");
			return ESP_ERR_TIMEOUT;
		}
		count++;
		ets_delay_us(1);
	}
	count = 0;
	while(gpio_get_level(DHT_PIN) == 1)
	{
		if(count > 80)
		{	
			printf("%d%s\n", count, "  DHT11 is retarded");
			 fflush (stdout);
			return ESP_ERR_TIMEOUT;
		}
		if(gpio_get_level(DHT_PIN) == 0)
			break;
		count++;
		ets_delay_us(1);
	}


	bitCount = 0;
	while(bitCount < 40){
	
		count = 0;
		while(gpio_get_level(DHT_PIN) == 0)
		{
			if (count > 50)
			{
				printf("%s\n", "DHT11 is retarded");
				return ESP_ERR_TIMEOUT;
			}
			if(gpio_get_level(DHT_PIN) == 1)
				break;
			count++;
			ets_delay_us(1);
		}
		count = 0;
		while(gpio_get_level(DHT_PIN) == 1)
		{
			if (count > 70)
			{
				printf("%s\n", "DHT11 is retarded");
				return ESP_ERR_TIMEOUT;
			}
			if(gpio_get_level(DHT_PIN) == 0)
				break;
			count++;
			ets_delay_us(1);
		}
  	  
		if (count > 28)
			dataBytes[byteNum] |= (1 << bitNum);
			
		if(bitNum == 0)
		{
			byteNum += 1;
			bitNum = 7;
		}
		else
			bitNum -= 1;
		
		bitCount++; 	  
  
	}




int checksum = dataBytes[0] + dataBytes[2] + dataBytes[3];

if(checksum != dataBytes[4]){
	printf("%s", "checksum is corrupted");
	return -1;
}

	printf("Temperature: %d C\n", dataBytes[2]);
	printf("Humidity: %d %%\n", dataBytes[0]);

	return (0);
}

void app_main(void)
{
	gpio_set_direction(DHT_POWER, GPIO_MODE_OUTPUT);	
	gpio_set_level(DHT_POWER, 1);
	vTaskDelay(2000 / portTICK_PERIOD_MS);
		
	 while (true){

		get_all_data();
	 	vTaskDelay(3000 / portTICK_PERIOD_MS);
	 }
}


