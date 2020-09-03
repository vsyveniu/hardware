#include "main.h"

#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13
#define SPI_CS GPIO_NUM_15
#define SPI_INT1_PIN GPIO_NUM_34
#define LED_1 GPIO_NUM_26
#define LED_2 GPIO_NUM_27
#define LED_3 GPIO_NUM_33
#define EN_AMP 5

xQueueHandle interruptQueue;

static void IRAM_ATTR spi_int_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue, &pin, NULL);
}

void leds_on(bool is_on){

	if(is_on){
		gpio_set_level(LED_1, 1);
		gpio_set_level(LED_2, 1);
		gpio_set_level(LED_3, 1);
	}
	else{
		gpio_set_level(LED_1, 0);
		gpio_set_level(LED_2, 0);
		gpio_set_level(LED_3, 0);
	}
}

void beep(){

	int volt;
	int beep_count;

		 for (beep_count = 0; beep_count < 1000; beep_count++){ 

              for (volt = 0; volt  < 256; volt+=20)
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
			
		 for (beep_count = 0; beep_count < 1000; beep_count++){ 

              for (volt = 0; volt  < 256; volt+=20)
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
			
		 for (beep_count = 0; beep_count < 1000; beep_count++){ 

            for (volt = 0; volt  < 256; volt+=20)
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
}

void accel_interrupt(void *sp){

	uint8_t pin;
	double x, y, z;

	static uint8_t z_fired = 0;
	static uint8_t x_fired = 0;
	static uint8_t y_fired = 0;
    _Bool is_x_low; 
    _Bool is_y_low;
    _Bool is_z_low;

   
	int16_t buff[3];

	for(int i = 0; i < 3; i++){
			buff[i] = 0x00;
	}

	spi_transaction_t adx_readData = {
		.cmd = 0x80 | 0x40 | 0x32,
		.length = 3 * 16,
		.rx_buffer = buff,
	};

	uint8_t int_src_buff[1];
	memset(int_src_buff, 0, sizeof(int_src_buff));

	spi_transaction_t int_src = {
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff, 
	};

	while (true){
		
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			spi_device_polling_transmit(sp, &adx_readData); 
			spi_device_polling_transmit(sp, &int_src);  // after reading data should read interrupt source register to clear it for next interrupts can be fired

			x = (double)buff[0]/256.0;
			y = (double)buff[1]/256.0;
			z = (double)buff[2]/256.0;

			is_x_low = (fabs(x) < 0.15);
    	    is_y_low = (fabs(y) < 0.15);
    	    is_z_low = (fabs(z) < 0.15);

			if((x < 1.3 && y < 1.3 && z < 1.3) && (x > -1.3 && y > -1.3 && z > -1.3)){

				if (is_z_low && is_y_low && !z_fired && !is_x_low)
				{
					leds_on(true);	
					z_fired = 1;
					beep();
				}
				else if(!is_z_low && is_y_low && !is_x_low )
				{
					leds_on(false);
					z_fired = 0;
				}
		
				else if (is_z_low && is_x_low && !x_fired && !is_y_low)
				{
					leds_on(true);
					x_fired = 1;
					beep();
				}
				else if(!is_z_low && is_x_low && !is_y_low)
				{
					leds_on(false);
					x_fired = 0;
				}

				else if ((is_z_low && !is_x_low && !y_fired))
				{
					leds_on(true);
					y_fired = 1;
					beep();
				}
				else if((!is_z_low && !is_x_low && !is_y_low))
				{
					leds_on(false);
					y_fired = 0;
				}
			}
		}
	}
}

void accel_init(spi_device_handle_t spi){

	int8_t adx_setbw_buff[1];
	adx_setbw_buff[0] = 0b00001010;   // set bw rate to 100hz output

	spi_transaction_t adx_setbw = {
		.cmd = 0x2Cu,
		.length = 8,
		.tx_buffer = adx_setbw_buff
	};
 
	uint8_t powctlbuff[1];
	powctlbuff[0] = 0b0001000;  // set link,measure,sleep,wakeup stuff. We need measurement mode

	spi_transaction_t adx_set_pow_ctl = {
		.cmd = 0x2Du,
		.tx_buffer = powctlbuff,
		.length = 8,
	};

	uint8_t tresh_buff[1];
	tresh_buff[0] = 5;  //set tresh_act register. When activity is detected, the magnitude compared with this value. If magnitude exceeds a value, interrupt is fired

	spi_transaction_t adx_treshhold = {
		.cmd = 0x24u,
		.length = 8,
		.tx_buffer = tresh_buff
	};

	uint8_t dataformat_buff[1];
	dataformat_buff[0] = 0b00100000;  //set dataformat. Bit 1 sets interrupt to fire in case 1 to 0, posedge in this case 

	spi_transaction_t adx_dataformat = {
		.cmd = 0x24u,
		.length = 8,
		.tx_buffer = dataformat_buff
	};

	uint8_t en_int1_buff[1];
	en_int1_buff[0] = 0b00010000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  

	spi_transaction_t adx_int1_en = {
		.cmd = 0x2Eu,
		.length = 8,
		.tx_buffer = en_int1_buff
	};

	uint8_t int_map_buff[1];
	int_map_buff[0] = 0b11101111;   // any bits set to 0 send interrupt to INT1 pin

	spi_transaction_t adx_int_map = {
		.cmd = 0x2Fu,
		.length = 8,
		.tx_buffer = int_map_buff
	};

	uint8_t act_inact_ctl[1];
	act_inact_ctl[0] = 0b11110000;    // activate or deactivate axis interrupts

	spi_transaction_t  adx_act_inact = {  
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = act_inact_ctl
	};
	
	uint8_t int_src_buff[1];

	spi_transaction_t clear_int_src = {  //read interrupts source register to clear it
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff
	};

	spi_device_polling_transmit(spi, &adx_setbw);
	spi_device_polling_transmit(spi, &adx_dataformat);
	spi_device_polling_transmit(spi, &adx_treshhold);
	spi_device_polling_transmit(spi, &adx_act_inact);
	spi_device_polling_transmit(spi, &adx_int_map);
	spi_device_polling_transmit(spi, &adx_int1_en);
	spi_device_polling_transmit(spi, &adx_set_pow_ctl);
	spi_device_polling_transmit(spi, &clear_int_src);
}

void accel_keep_alive_crutch(void *spi){

	uint8_t int_src_buff[1];
	memset(int_src_buff, 0, sizeof(int_src_buff));

	spi_transaction_t int_src = {
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff, 
	};
	while(true){
		vTaskDelay(200 / portTICK_PERIOD_MS);
		spi_device_polling_transmit(spi, &int_src);
	}
}


void app_main(void){

	esp_err_t err;

	gpio_set_direction(EN_ACCEL, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_3, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_ACCEL, 1);
	gpio_set_direction(EN_AMP, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_AMP, 1);
    dac_output_enable(DAC_CHANNEL_1);
	vTaskDelay(1000 / portTICK_PERIOD_MS);


	gpio_config_t int_conf = {
		.pin_bit_mask = GPIO_SEL_34,
		.mode = GPIO_MODE_INPUT,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config(&int_conf);

	spi_device_handle_t spi;

	spi_bus_config_t spi_bus_conf = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
	};

	spi_device_interface_config_t spi_device_conf = {
 		.command_bits = 8,
		.mode = 3,
		.clock_speed_hz = 1000000,
		.spics_io_num = SPI_CS,
		.queue_size = 1,
	};

	err = spi_bus_initialize(SPI3_HOST, &spi_bus_conf, 0);
	ESP_ERROR_CHECK(err);

	err = spi_bus_add_device(SPI3_HOST, &spi_device_conf, &spi);
	ESP_ERROR_CHECK(err);

	gpio_install_isr_service(0);

	gpio_isr_handler_add(SPI_INT1_PIN, spi_int_handler, (void *)SPI_INT1_PIN);

	interruptQueue = xQueueCreate(10, sizeof(int));

	xTaskCreate(accel_interrupt, "accell task", 2048, spi, 1, NULL);
	xTaskCreate(accel_keep_alive_crutch, "accell crutch task", 2048, spi, 1, NULL);

	accel_init(spi);

}


