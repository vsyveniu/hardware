#include "main.h"
  
#define LED_27 27
#define LED_26 26
#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_1
#define LEDC_TEST_DUTY         (8000)


void app_main(void)
{

 	ledc_timer_config_t ledc_timer = {
 		.speed_mode = LEDC_SPEED_MODE,
 		.timer_num = LEDC_TIMER_1,
 		.duty_resolution = LEDC_TIMER_13_BIT,
 		.freq_hz = 5000,
 	};

	ledc_timer_config(&ledc_timer);		

	ledc_channel_config_t ledc_channel = {
		.channel = LEDC_CHANNEL_0,
		.speed_mode = LEDC_SPEED_MODE,
		.gpio_num = LED_27,
		.duty = 0,
		.timer_sel = LEDC_TIMER_1,
		.hpoint = 0,
	};


	 ledc_channel_config(&ledc_channel);





	 	// ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, ch);
	 	// 	ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
	 	// 	ESP_LOGI("dutyloop", "%d", ledc_channel.duty);
	 	// 	ESP_LOGI("duteget", "%d", ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));


	 	// ESP_LOGI("channel", "%d", LEDC_CHANNEL_0);
	 	// ESP_LOGI("timer", "%d", ledc_timer.freq_hz);


	 	
	 	//ESP_LOGI("duty", "%d", ledc_channel.duty);
	 
	 	//ESP_LOGI("duty", "%d", ledc_channel.duty);

	 ledc_fade_func_install(0);

	 while (true){
	 

	 		ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, LEDC_TEST_DUTY, 8000);
	 		ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);
	 		//ledc_stop(ledc_channel.speed_mode, ledc_channel.channel, 0);

	 		// ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 8000);
	 		// ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);

	  }

}

