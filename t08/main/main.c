#include "main.h"
#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13
#define SPI_CS GPIO_NUM_15
#define SPI_INT1_PIN GPIO_NUM_34
#define LED_1 GPIO_NUM_26
#define LED_2 GPIO_NUM_27


xQueueHandle interruptQueue;

static void IRAM_ATTR spi_int_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue, &pin, NULL);
}


void beep(){

int volt;
int beep_count;

		 for (beep_count = 0; beep_count < 1000; beep_count++){ 

              for (volt = 0; volt  < 256; volt+=100)
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

              for (volt = 0; volt  < 256; volt+=100)
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

              for (volt = 0; volt  < 256; volt+=100)
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



void accel_interrupt(spi_device_handle_t spi){
	uint8_t pin;
	double x, y, z;



		int8_t bufftx[3];
		int16_t buff[3];
	for(int i = 0; i < 8; i++){
		buff[i] = 0x00;
	}

	spi_transaction_t adx_readData = {
		.cmd = 0x80 | 0x40 | 0x32,
		.length = 3 * 16,
		.tx_buffer = bufftx,
		.rx_buffer = buff,
	};

	uint8_t int_src_buff[1];

	spi_transaction_t int_src = {
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff, 
	};

	while (true){
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			printf("%s\n", "fuck interrupt");
			spi_device_polling_transmit(spi, &int_src);
			spi_device_polling_transmit(spi, &adx_readData);
			//printf("%d interrupt src \n", int_src_buff[0]);
			x = buff[0];
			y = buff[1];
			z = buff[2];

			//x = atan((double)x / (sqrt(pow((double)y,z) + pow((double)y,z))));

		 	printf("x angle  %f \n", x);
			printf("y angle  %f \n", y);
			printf("z angle  %f \n", z);


			if(x > 240 && y < 20){
				gpio_set_level(LED_1, 1);
				gpio_set_level(LED_2, 1);
				vTaskDelay(100 / portTICK_PERIOD_MS);
				gpio_set_level(LED_1, 0);
				gpio_set_level(LED_2, 0);
			}
		}
	}
}


       /*  double angle_y = atan((double)accs[1] / (sqrt(pow((double)accs[0],2) + pow((double)accs[2],2))));
        double angle_z = atan((double)accs[2] / (sqrt(pow((double)accs[0],2) + pow((double)accs[1],2)))); */



void app_main(void){

	esp_err_t err;

	gpio_set_direction(EN_ACCEL, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_ACCEL, 1);
	vTaskDelay(5 / portTICK_PERIOD_MS);



	gpio_config_t int_conf = {
		.pin_bit_mask = GPIO_SEL_34,
		.mode = GPIO_MODE_INPUT,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config(&int_conf);
	//gpio_set_level(SPI_INT1_PIN, 0);

	spi_device_handle_t spi;


	 spi_bus_config_t spi_bus_conf = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		//.max_transfer_sz = 32,
/* 		.flags=,
		.intr_flags=, */

	};


	spi_device_interface_config_t spi_device_conf = {
 		.command_bits = 8,
		//.address_bits = 0,
		//.dummy_bits = 0, 
		.mode = 3,
		/* .duty_cycle_pos = 0,
		.cs_ena_pretrans= 0,
		.cs_ena_posttrans= 0, */
		.clock_speed_hz = 1000000,
		//.input_delay_ns = 0,
		.spics_io_num = SPI_CS,
		//.flags = 0,
		.queue_size = 1,
		//.pre_cb = NULL,
		//.post_cb = NULL,

	};

	int16_t buff[3];
	for(int i = 0; i < 8; i++){
		buff[i] = 0x00;
	}


	int8_t adx_setbwbuff[1];

	adx_setbwbuff[0] = 0b00001010;   // set bw rate to 100hz output
	spi_transaction_t adx_setbw = {
		.flags = SPI_TRANS_USE_RXDATA,
		.cmd = 0x2Cu,
		.length = 8,
	};
 
	uint8_t powctlbuff[1];

	powctlbuff[0] = 0b0001000;  // set /link/measure/sleep/wakeup shit
	spi_transaction_t adx_set_pow_ctl = {
		.cmd = 0x2Du,
		.tx_buffer = powctlbuff,
		.length = 8,
	};

	uint8_t powctlbuff_r[1];

	spi_transaction_t adx_read_bw = {
		.cmd = 0x80 | 0x40 | 0x2Du,
		.rx_buffer = powctlbuff_r,
		.length = 8,
	};


	uint8_t treshbuff[1];

	treshbuff[0] = 5;  //set tresh_act register
	spi_transaction_t adx_treshhold = {
		.cmd = 0x24u,
		.length = 8,
		.tx_buffer = treshbuff
	};


	uint8_t dataformatbuff[1];

	dataformatbuff[0] = 0b00100000;  //set dataformat
	spi_transaction_t adx_dataformat = {
		.cmd = 0x24u,
		.length = 8,
		.tx_buffer = dataformatbuff
	};



	uint8_t intbuff_r[1];

	spi_transaction_t tresh_read = {
		.cmd = 0x80 | 0x40 | 0x24u,
		.length =  3 * 16,
		.rx_buffer = intbuff_r,
	};

	uint8_t tresh_inact_buff[1];

	tresh_inact_buff[0] = 1;   //set tresh_inact register
	spi_transaction_t adx_inact_treshhold = {
		.cmd = 0x25u,
		.length = 8,
		.tx_buffer = tresh_inact_buff
	};

	uint8_t inact_intbuff_r[3];

	spi_transaction_t tresh_inact_read = {
		.cmd = 0x80 | 0x40 | 0x25u,
		.length =  3 * 8,
		.rx_buffer = inact_intbuff_r,
	};



	uint8_t en_int1_buff[1];

	en_int1_buff[0] = 0b00010000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  

	spi_transaction_t adx_int1_en = {
		.cmd = 0x2Eu,
		.length = 8,
		.tx_buffer = en_int1_buff
	};


	uint8_t en_int1buffR[1];

	spi_transaction_t en1_readdata = {
		.cmd = 0x80 | 0x40 | 0x2Eu,
		.length = 8,
		.rx_buffer = en_int1buffR,
	};

	uint8_t int_map_buff[1];

	int_map_buff[0] = 0b11101111;   // any bits set to 0 send interrupt to INT1 pin
	spi_transaction_t adx_int_map = {
		.cmd = 0x2Fu,
		.length = 8,
		.tx_buffer = int_map_buff
	};

	uint8_t int_map_buff_R[1];

	spi_transaction_t int_map_read = {
		.cmd = 0x80 | 0x40 | 0x2F,
		.length = 8,
		.rx_buffer = int_map_buff_R,
	};


	uint8_t int_src_buff[1];

	spi_transaction_t int_src = {
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff
	};

	uint8_t act_inact_ctl[1];
	act_inact_ctl[0] = 0b11000000;     

	spi_transaction_t  adx_act_inact = {
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = act_inact_ctl
	};


	uint8_t act_inact_read_buff[1];

	spi_transaction_t act_inact_read = {
		.cmd = 0x80 | 0x40 | 0x27u,
		.length = 8,
		.rx_buffer = act_inact_read_buff,
	};

	

	int8_t bufftx[3];
	spi_transaction_t adx_readData = {
		//.flags = SPI_TRANS_USE_RXDATA,
		.cmd = 0x80 | 0x40 | 0x32,
		.length = 3 * 16,
		.tx_buffer = bufftx,
		.rx_buffer = buff,
	};

	err = spi_bus_initialize(SPI3_HOST, &spi_bus_conf, 0);
	ESP_ERROR_CHECK(err);

	err = spi_bus_add_device(SPI3_HOST, &spi_device_conf, &spi);
	ESP_ERROR_CHECK(err);

	///10000  int en
	//10000011 131
	//10010000
	//10010011 147
	//10010010 146 dready/activity/watermark
	//11101111 239 map
	//10000010 130
	//10 2 / watermark
	//10000 16
	//1111000 act inact
	// 00010010 activity / watermark
	//1011   11

	//11000  24

	//111000 56

	//10001011

	//11001100

	


	gpio_install_isr_service(0);

	gpio_isr_handler_add(SPI_INT1_PIN, spi_int_handler, (void *)SPI_INT1_PIN);

	interruptQueue = xQueueCreate(10, sizeof(int));


	xTaskCreate(accel_interrupt, "accell 1 task", 2048, spi, 1, NULL);

	spi_device_polling_transmit(spi, &adx_setbw);
	spi_device_polling_transmit(spi, &adx_dataformat);
	spi_device_polling_transmit(spi, &adx_treshhold);
	spi_device_polling_transmit(spi, &tresh_read);
	spi_device_polling_transmit(spi, &adx_inact_treshhold);
	spi_device_polling_transmit(spi, &en1_readdata);
	spi_device_polling_transmit(spi, &adx_act_inact);
	spi_device_polling_transmit(spi, &act_inact_read);
	spi_device_polling_transmit(spi, &int_map_read);
	spi_device_polling_transmit(spi, &adx_int_map);
	spi_device_polling_transmit(spi, &int_src);
	spi_device_polling_transmit(spi, &tresh_inact_read);
	spi_device_polling_transmit(spi, &adx_int1_en);

	printf("%d treshhold act\n", intbuff_r[0]);
	printf("%d treshhold inact\n", tresh_inact_buff[0]);
 	printf("%d interrupt en \n", en_int1buffR[0]);
	printf("%d interrupt map \n", int_map_buff_R[0]);
	printf("%d act inact \n", act_inact_read_buff[0]); 
	spi_device_polling_transmit(spi, &adx_set_pow_ctl);
	spi_device_polling_transmit(spi, &adx_read_bw);
	printf("%d pw ctl \n", powctlbuff_r[0]);

	while (true)
	{
		//printf("%s\n", "----------");
		vTaskDelay(200 / portTICK_PERIOD_MS);
		//spi_device_polling_transmit(spi, &adx_readData);

		//spi_device_polling_transmit(spi, &int_src);
		//if(int_src_buff[0] == 18 || int_src_buff[0] == 146)
			//printf("%d interrupt src \n", int_src_buff[0]);
			//printf("%d\n", gpio_get_level(SPI_INT1_PIN));





		/* x_angle = atan2(y, z) * 180/M_PI;
		y_angle = atan2(x, z) * 180/M_PI;
		z_angle = atan2(y, x) * 180/M_PI; */

	 	  // printf("x angle  %d \n", x);
		//printf("y angle  %d \n", y);
//		printf("z angle  %d \n", z);   


		/* if((x > 230 || y > 230 || z > 230) || (x < -230 || y < -230 || z < -230)){


			printf("%s", "fuck");

		}  */


	}

}


