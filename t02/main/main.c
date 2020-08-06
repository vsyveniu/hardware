#include "main.h"
  
#define LED_27 27
#define LED_26 26
#define LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_1
#define LEDC_TEST_DUTY         (8000)


void app_main(void)
{
	uint32_t duty;

 	ledc_timer_config_t ledc_timer = {
 		.speed_mode = LEDC_SPEED_MODE,
 		.timer_num = LEDC_TIMER_1,
 		.duty_resolution = LEDC_TIMER_10_BIT,
 		.freq_hz = 300,
 	};

 	ledc_timer_config_t ledc_timer_2 = {
 		.speed_mode = LEDC_SPEED_MODE,
 		.timer_num = LEDC_TIMER_2,
 		.duty_resolution = LEDC_TIMER_10_BIT,
 		.freq_hz = 300,
 	};

	ledc_timer_config(&ledc_timer);
	ledc_timer_config(&ledc_timer_2);			

	ledc_channel_config_t ledc_channel = {
		.channel = LEDC_CHANNEL_0,
		.speed_mode = LEDC_SPEED_MODE,
		.gpio_num = LED_27,
		.duty = 0,
		.timer_sel = LEDC_TIMER_1,
		.hpoint = 0,
	};

	ledc_channel_config_t ledc_channel_2 = {
		.channel = LEDC_CHANNEL_1,
		.speed_mode = LEDC_SPEED_MODE,
		.gpio_num = LED_26,
		.duty = 0,
		.timer_sel = LEDC_TIMER_2,
		.hpoint = 0,
	};


	 ledc_channel_config(&ledc_channel);
	 ledc_channel_config(&ledc_channel_2);


	 dac_cw_config_t dac_conf = {
	 	.en_ch = DAC_CHANNEL_2,
	 	.dac_cw_scale_t = DAC_CW_SCALE_8,
	 	.dac_cw_phase_r = DAC_CW_PHASE_180,
	 	.freq = 300,
	 	.offset = 20,

	 }

	 	// ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, ch);
	 	// 	ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
	 	// 	ESP_LOGI("dutyloop", "%d", ledc_channel.duty);
	 	// 	ESP_LOGI("duteget", "%d", ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));


	 	// ESP_LOGI("channel", "%d", LEDC_CHANNEL_0);
	 	// ESP_LOGI("timer", "%d", ledc_timer.freq_hz);


	 	
	 	//ESP_LOGI("duty", "%d", ledc_channel.duty);
	 
	 	//ESP_LOGI("duty", "%d", ledc_channel.duty);

	 ledc_fade_func_install(0);


	 dac_output_enable(DAC_CHANNEL_2);
	 dac_cw_generator_enable();
	 dac_cw_generator_config(&dac_conf);
	 

	 while (true){ 

	 		//dac_output_voltage(DAC_CHANNEL_2, 200);
	 
	 	for(duty = 0; duty < 1023; duty++)
	 		{

	 			ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
	 		 	ets_delay_us(700);
				ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);


				//ledc_set_fade_with_time(ledc_channel_2.speed_mode, ledc_channel_2.channel, duty, 2);
				//ledc_fade_start(ledc_channel_2.speed_mode, ledc_channel_2.channel, LEDC_FADE_NO_WAIT);

				//ledc_set_freq();
		
				// ESP_LOGI("duty", "%d", ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));
				//ESP_LOGI("duty", "%d", ledc_get_freq(ledc_channel.speed_mode, ledc_timer.timer_num));
				// ledc_set_fade_with_time(ledc_channel_2.speed_mode, ledc_channel_2.channel, duty, 7);
				// ledc_fade_start(ledc_channel_2.speed_mode, ledc_channel_2.channel, LEDC_FADE_NO_WAIT);

			}
			//ESP_LOGI("duty", "%d", ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));
			//ets_delay_us(300);
			for(duty = 1023; duty > 0; duty--)
	 		{

	 			ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
			 	ets_delay_us(700);
				ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);


				// ledc_set_fade_with_time(ledc_channel_2.speed_mode, ledc_channel_2.channel, duty, 7);
				// ledc_fade_start(ledc_channel_2.speed_mode, ledc_channel_2.channel, LEDC_FADE_NO_WAIT);
				

				//ledc_set_freq();
		
				// ESP_LOGI("duty", "%d", ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));
				//ESP_LOGI("duty", "%d", ledc_get_freq(ledc_channel.speed_mode, ledc_timer.timer_num));
			}
			//ESP_LOGI("duty", "%d", ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));









	 		//ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, LEDC_TEST_DUTY, 8000);
	 		//ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);
	 		//ledc_stop(ledc_channel.speed_mode, ledc_channel.channel, 0);

	 		// ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 8000);
	 		// ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);

	  }

}

