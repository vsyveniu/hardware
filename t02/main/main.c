#include "main.h"
  
#define LED_1 27
#define LED_2 26
#define LEDC_SPEED_MODE 		LEDC_HIGH_SPEED_MODE
#define LEDC_DUTY  				(1023)
#define LEDC_DUTY_RESOLUTION	LEDC_TIMER_10_BIT
#define LEDC_FREQENCY			(1000)


void pulse_led1(ledc_channel_config_t ledc_channel){

	// uint32_t duty;

 // 	for(duty = 0; duty < LEDC_DUTY; duty++)
 // 	{
				 	
	// 	ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
	//  	ets_delay_us(220);
	// 	ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
	// }

	// for(duty = LEDC_DUTY; duty > 0; duty--)
	// {  
	// 	ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
	// 	ets_delay_us(220);
	// 	ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
	// }
	// ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 0);
	// ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);



	ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, LEDC_DUTY, 2000);
	ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
	ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 2000);
	ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
}


void pulse_led2(){

	int volt;

	dac_output_enable(DAC_CHANNEL_2);

	for (volt = 0; volt  < 255; volt++)
	{
		dac_output_voltage(DAC_CHANNEL_2, volt);
		ets_delay_us(8000);
	}

	for (volt = 255; volt  > 0; volt--)
	{
		dac_output_voltage(DAC_CHANNEL_2, volt);
		ets_delay_us(8000);
	}
			 
}

void app_main(void)
{
	ledc_timer_config_t ledc_timer = {
 		.speed_mode = LEDC_SPEED_MODE,
 		.timer_num = LEDC_TIMER_1,
 		.duty_resolution = LEDC_DUTY_RESOLUTION,
 		.freq_hz = LEDC_FREQENCY,
 	};

	ledc_timer_config(&ledc_timer);		

	ledc_channel_config_t ledc_channel = {
		.channel = LEDC_CHANNEL_0,
		.speed_mode = LEDC_SPEED_MODE,
		.gpio_num = LED_1,
		.duty = 0,
		.timer_sel = LEDC_TIMER_1,
		.hpoint = 0,
	};

	ledc_channel_config(&ledc_channel);
	ledc_fade_func_install(0);

	while(true){
		pulse_led1(ledc_channel);
		pulse_led2();
	}
}


