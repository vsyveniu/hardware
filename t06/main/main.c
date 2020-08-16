#include "main.h"
#define EN_AMP 5




void app_main(void)
{

    gpio_set_direction(EN_AMP, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_AMP, 1);
    vTaskDelay( 2 / portTICK_PERIOD_MS);
  
    //dac_output_enable(DAC_CHANNEL_1);


  /*   dac_cw_config_t cw_config = {
        .en_ch = DAC_CHANNEL_1,
        .scale = DAC_CW_SCALE_1,
        .phase = DAC_CW_PHASE_0,
        .freq = 130,
        .offset = 100 
    } */

  /*   dac_cw_generator_config(&cw_config);
    dac_cw_generator_enable(); */
    
    sigmadelta_config_t sigmadelta_cfg = {
        .channel = SIGMADELTA_CHANNEL_0,
        .sigmadelta_prescale = 255,
        .sigmadelta_duty = 0,
        .sigmadelta_gpio = GPIO_NUM_25,
    };

    sigmadelta_config(&sigmadelta_cfg);

     int8_t duty = 0;
    int inc = 1;

	 while (true){

         gpio_set_level(EN_AMP, 1);

        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, 20);
         
        vTaskDelay(100 / portTICK_PERIOD_MS);

        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, 50);

         vTaskDelay(100 / portTICK_PERIOD_MS);

        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, 80);

         vTaskDelay(100 / portTICK_PERIOD_MS);
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, 120);

        
        gpio_set_level(EN_AMP, 0);
        vTaskDelay(10000 / portTICK_PERIOD_MS);

       /*  duty += inc;
        if (duty == 127 || duty == -127) {
            inc = (-1) * inc;
        } */

        //int volt;
        //dac_output_voltage(DAC_CHANNEL_1, 200);


	/* for (volt = 0; volt  < 255; volt++)
	{
		dac_output_voltage(DAC_CHANNEL_1, volt);
		ets_delay_us(8000);
	}

	for (volt = 255; volt  > 0; volt--)
	{
		dac_output_voltage(DAC_CHANNEL_1, volt);
		ets_delay_us(8000);
	} */

	 }
}


