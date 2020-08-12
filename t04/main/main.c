#include "main.h"
#define DHT_PIN 	GPIO_NUM_4



void sendStart(){

	gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(DHT_PIN, 0);
	//vTaskDelay(25 / portTICK_PERIOD_MS);
	ets_delay_us(22000);
	// printf("%d\n", gpio_get_level(DHT_PIN));
	// printf("%s\n", "level 0 -------");
	gpio_set_level(DHT_PIN, 1);
	ets_delay_us(53);
	// printf("%s\n", " level 1------");

	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);

}


int get_humidity(){

	sendStart();

	int count = 0;

	printf("%d\n", gpio_get_level(DHT_PIN));


	ets_delay_us(400);

	// while(gpio_get_level(DHT_PIN) == 1){
	// 	if(count > 400){
	// 		//ESP_LOGI("req counter", "%d, %s", count, "timeout");
	// 		printf("%s\n", "timeout 40us");
	// 		return ESP_ERR_TIMEOUT;
	// 	}
	// 	// if(gpio_get_level(DHT_PIN) == 0){
	// 	// 	printf("%s\n", "response 40 us");
	// 	// 	break;
	// 	// }
	// 	count++;
	// 	ets_delay_us(1);
	// }
	printf("%d%s\n", gpio_get_level(DHT_PIN), "     dht11 send response");



	//printf("%d\n", gpio_get_level(DHT_PIN));

	count = 0;


	ets_delay_us(400);
	printf("%d\n", gpio_get_level(DHT_PIN));
	ets_delay_us(400);
	printf("%d\n", gpio_get_level(DHT_PIN));

	// while(gpio_get_level(DHT_PIN) == 0){
	// 	if(count > 120){
	// 		printf("%s\n", "timeout 80us");
	// 		return ESP_ERR_TIMEOUT;
	// 	}
	// 	// if(gpio_get_level(DHT_PIN) == 1){
	// 	// 	printf("%s\n", "response2");
	// 	// 	break;
	// 	// }
	// 	count++;
	// 	ets_delay_us(1);
	// }

	


	//printf("%d\n", gpio_get_level(DHT_PIN));
	// do{
	// 	if(req_count_low > 80){
	// 			printf("%s\n", "timeout low  response 80 us");
	//  		return ESP_ERR_TIMEOUT;
	// 	}
	// 	req_count_low++;
	// 	ets_delay_us(1);
	// 	//printf("%d\n", req_count_low);
	// }while(gpio_get_level(DHT_PIN) != 1);


	//printf("%d\n", gpio_get_level(DHT_PIN));
	// //printf("%d\n", gpio_get_level(DHT_PIN));

	// while(gpio_get_level(DHT_PIN) == 1){
	// 	if(resp_count_high > 80){
	// 			printf("%s\n", "timeout high response 80 us");
	// 		return ESP_ERR_TIMEOUT;
	// 	}
	// 	if(gpio_get_level(DHT_PIN) == 0){
	// 		printf("%s\n", "response3");
	// 		break;

	// 	}
	// 	resp_count_high++;
	// 	ets_delay_us(1);
	// }
	//printf("%d\n", gpio_get_level(DHT_PIN));


	return 0;
}

int get_temperature(){

	return 0;
}




void app_main(void)
{
	// gpio_config_t butt_1_conf = {
	// 	.pin_bit_mask = GPIO_SEL_4,
	// 	.mode = GPIO_MODE_INPUT,
	// 	.pull_up_en = GPIO_PULLUP_ENABLE,
	// 	.pull_down_en = GPIO_PULLDOWN_DISABLE,
	// 	.intr_type = GPIO_INTR_POSEDGE,
	// };

	// gpio_config(&butt_1_conf);

	 while (true){

	 	//printf("%d\n", get_temperature());
	 	vTaskDelay(1000 / portTICK_PERIOD_MS);
	 	get_humidity();
	 	//printf("%d\n", get_humidity());

	 }
}


