#include "main.h"
#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13
#define SPI_CS GPIO_NUM_15
#define SPI_INT1_PIN GPIO_NUM_34
#define SPI_INT2_PIN GPIO_NUM_35
#define LED_1 GPIO_NUM_26
#define LED_2 GPIO_NUM_27
#define LED_3 GPIO_NUM_33
#define EN_AMP 5


xQueueHandle interruptQueue;
xQueueHandle interrupt2Queue;

static void IRAM_ATTR spi_int_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interruptQueue, &pin, NULL);
}

static void IRAM_ATTR spi_int2_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(interrupt2Queue, &pin, NULL);
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

void tap_interrupt(spi_device_handle_t spi){
	uint8_t pin = 0;

	uint8_t en_x_axis_buff[1];

	en_x_axis_buff[0] = 0b11110000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  

	spi_transaction_t en_x_axis = {
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = en_x_axis_buff
	};

	uint8_t dis_x_axis_buff[1];
	
	dis_x_axis_buff[0] = 0b10010000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  
 
	spi_transaction_t  dis_x_axis = {  // activate or deactivate axis interrupts
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = dis_x_axis_buff
	};

	uint8_t int_src_buff[1];

	spi_transaction_t int_src = {
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff, 
	};


	while (true){
		if(xQueueReceive(interrupt2Queue, &pin, portMAX_DELAY))
		{
			printf("%s\n", "this is int 2");

		}
	}
}

void accel_interrupt(spi_device_handle_t spi){
	uint8_t pin;
	double x, y, z;
	uint8_t count = 0;


	static uint8_t z_fired;
	static uint8_t x_fired;
	static uint8_t y_fired;
	z_fired = 0;
	x_fired = 0;
	y_fired = 0;
	static uint8_t z_lock;
	static uint8_t x_lock;
	static uint8_t y_lock;
	z_lock = 0;
	x_lock = 0;
	y_lock = 0;

	uint8_t tmp = 0;
    _Bool is_x_low; 
    _Bool is_y_low;
    _Bool is_z_low;
	uint8_t flags = 0b00000000;
   



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

	uint8_t en_all_axis_buff[1];

	en_all_axis_buff[0] = 0b11110000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  

	spi_transaction_t en_all_axis = {
		.cmd = 0x2Eu,
		.length = 8,
		.tx_buffer = en_all_axis_buff
	};

	uint8_t active_z_axis_buff[1];
	
	active_z_axis_buff[0] = 0b10010000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  
 
	spi_transaction_t  active_z_axis = {  // activate or deactivate axis interrupts
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = active_z_axis_buff
	};

	uint8_t active_y_axis_buff[1];
	
	active_y_axis_buff[0] = 0b10100000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  
 
	spi_transaction_t  active_y_axis = {  // activate or deactivate axis interrupts
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = active_y_axis_buff
	};

	uint8_t active_x_axis_buff[1];
	
	active_x_axis_buff[0] = 0b11000000;  // enable interrupts on type corresponds to it's bit num and level. Activity interrupt  
 
	spi_transaction_t  active_x_axis = {  // activate or deactivate axis interrupts
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = active_x_axis_buff
	};

	uint8_t tap_status_buff[1];

	for(int k = 0; k < 8; k++){
		tap_status_buff[k] = 0;
	}

	spi_transaction_t tap_status = {
		.cmd = 0x80 | 0x40 | 0x2Bu,
		.length = 8,
		.rx_buffer = tap_status_buff, 
	};


	uint8_t tap_dis_axis_buff[1];

	tap_dis_axis_buff[0] = 0b00001000;  //tap azxes
	spi_transaction_t tap_dis_axis = {
		.cmd = 0x2Au,
		.length = 8,
		.tx_buffer = tap_dis_axis_buff
	};

	uint8_t tap_en_axis_buff[1];

	tap_en_axis_buff[0] = 0b00001001;  //tap azxes
	spi_transaction_t tap_en_axis = {
		.cmd = 0x2Au,
		.length = 8,
		.tx_buffer = tap_en_axis_buff
	};


	while (true){
		
		if(xQueueReceive(interruptQueue, &pin, portMAX_DELAY))
		{
			
			printf("%s z \n", "fuck interrupt");
			printf("%d count \n", count);
			//printf("%d damn \n", damn);

			
			//spi_device_polling_transmit(spi, &tap_status);
			spi_device_polling_transmit(spi, &adx_readData);
			spi_device_polling_transmit(spi, &int_src);
			//printf("%d interrupt src \n", int_src_buff[0]);
			//printf("%d tap status \n", tap_status_buff[0]);

			x = (double)buff[0]/256.0;
			y = (double)buff[1]/256.0;
			z = (double)buff[2]/256.0;

		/* 	printf("%f\n", x);
			printf("%f\n", y); 
			printf("%f\n", z);
 */

			is_x_low = (fabs(x) < 0.15);
    	    is_y_low = (fabs(y) < 0.15);
    	    is_z_low = (fabs(z) < 0.15);
			

			printf("%d\n", is_x_low);
			printf("%d\n", is_y_low);
			printf("%d\n", is_z_low);
/* 			if(is_z_low)
				z_fired = 1;
			else	
				z_fired = 0;
			if(is_y_low)
				y_fired = 1;
			else	
				y_fired = 0;
			if(is_x_low)
				x_fired = 1;
			else	
				x_fired = 0;	
 */
			if (is_z_low && is_y_low && !z_fired && !is_x_low)
    		{
				printf("%s\n", "sdf");
				
				gpio_set_level(LED_1, 1);
				gpio_set_level(LED_2, 1);
				gpio_set_level(LED_3, 1);
				z_fired = 1;
				y_fired = 0;
				x_lock = 1;
				y_lock = 1;
				printf("%d z\n", z_fired);
				beep(true);
				//spi_device_polling_transmit(spi, &active_z_axis);
				//spi_device_polling_transmit(spi, &int_src);
				count = 0;
			}
			else if(!is_z_low && is_y_low && !is_x_low )
			{
				gpio_set_level(LED_1, 0);
				gpio_set_level(LED_2, 0);
				gpio_set_level(LED_3, 0);
				z_fired = 0;
				y_fired = 0;
				printf("%d z\n", z_fired);
				//spi_device_polling_transmit(spi, &tap_en_axis);
				//damn = 0;
				count = 0;
				z_lock = 0;
				//spi_device_polling_transmit(spi, &en_all_axis);
				//spi_device_polling_transmit(spi, &int_src);
			}
	
			else if (is_z_low && is_x_low && !x_fired && !is_y_low)
    		{
				printf("%s\n", "ggggdf");
				
				gpio_set_level(LED_1, 1);
				gpio_set_level(LED_2, 1);
				gpio_set_level(LED_3, 1);
				x_fired = 1;
				printf("%d z\n", z_fired);
				beep();
				//spi_device_polling_transmit(spi, &active_x_axis);
				//spi_device_polling_transmit(spi, &int_src);
				count = 0;
			}
			else if(!is_z_low && is_x_low && !is_y_low)
			{
				gpio_set_level(LED_1, 0);
				gpio_set_level(LED_2, 0);
				gpio_set_level(LED_3, 0);
				x_fired = 0;

				printf("%d z\n", z_fired);
				//spi_device_polling_transmit(spi, &en_all_axis);
				//damn = 0;
				count = 0;
				z_lock = 0;
			}

			else if ((is_z_low && !is_x_low && !y_fired))
    		{
				printf("%s\n", "sddddf");
				
				gpio_set_level(LED_1, 1);
				gpio_set_level(LED_2, 1);
				gpio_set_level(LED_3, 1);
				y_fired = 1;
				printf("%d z\n", z_fired);
				beep();
				//spi_device_polling_transmit(spi, &active_x_axis);
				//spi_device_polling_transmit(spi, &int_src);
				count = 0;
			}
			else if((!is_z_low && !is_x_low && !is_y_low))
			{
				gpio_set_level(LED_1, 0);
				gpio_set_level(LED_2, 0);
				gpio_set_level(LED_3, 0);
				y_fired = 0;
				printf("%d z\n", z_fired);
				//spi_device_polling_transmit(spi, &en_all_axis);
				//damn = 0;
				count = 0;
				z_lock = 0;
			}
			/* else if(!is_z_low && !is_x_low && !is_y_low && z_fired)
			{
				gpio_set_level(LED_1, 0);
				gpio_set_level(LED_2, 0);
				gpio_set_level(LED_3, 0);
				z_fired = 0;
				x_fired = 0;
				y_fired = 0;
				printf("%d z\n", z_fired);
				//spi_device_polling_transmit(spi, &en_x_axis);
				//damn = 0;
			} */
			/* else if()
			{
				gpio_set_level(LED_1, 0);
				gpio_set_level(LED_2, 0);
				gpio_set_level(LED_3, 0);
				z_fired = 0;
				x_fired = 1;
				y_fired = 1;
				printf("%d z\n", z_fired);
				//spi_device_polling_transmit(spi, &en_x_axis);
				//damn = 0;
			} */
		}
	}
}


void accel_init(spi_device_handle_t spi){
		int16_t buff[3];
	for(int i = 0; i < 8; i++){
		buff[i] = 0x00;
	}


	int8_t adx_setbwbuff[1];

	adx_setbwbuff[0] = 0b00001010;   // set bw rate to 100hz output
	spi_transaction_t adx_setbw = {
		.cmd = 0x2Cu,
		.length = 8,
		.tx_buffer = adx_setbwbuff
	};
 
	uint8_t powctlbuff[1];

	powctlbuff[0] = 0b0001000;  // set /link/measure/sleep/wakeup shit
	spi_transaction_t adx_set_pow_ctl = {
		.cmd = 0x2Du,
		.tx_buffer = powctlbuff,
		.length = 8,
	};

	uint8_t treshTapBuff[1];

	treshTapBuff[0] = 3;  
	spi_transaction_t adx_set_tresh_tap = {
		.cmd = 0x1Du,
		.tx_buffer = treshTapBuff,
		.length = 8,
	};

	uint8_t DurBuff[1];

	DurBuff[0] = 10;  // dur
	spi_transaction_t adx_set_dur = {
		.cmd = 0x21u,
		.tx_buffer = DurBuff,
		.length = 8,
	};

	uint8_t treshbuff[1];

	treshbuff[0] = 5;  //set tresh_act register
	spi_transaction_t adx_treshhold = {
		.cmd = 0x24u,
		.length = 8,
		.tx_buffer = treshbuff
	};

	// 10000 16
	//10100
	//100
	//10101 21
	// 10100 20
	//10001

	//10001 17
	// 10010 18
	// 1000001
	//10100101
	uint8_t dataformatbuff[1];

	dataformatbuff[0] = 0b00100000;  //set dataformat
	spi_transaction_t adx_dataformat = {
		.cmd = 0x24u,
		.length = 8,
		.tx_buffer = dataformatbuff
	};

	uint8_t tresh_inact_buff[1];

	tresh_inact_buff[0] = 1;   //set tresh_inact register
	spi_transaction_t adx_inact_treshhold = {
		.cmd = 0x25u,
		.length = 8,
		.tx_buffer = tresh_inact_buff
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

	uint8_t tap_dis_buff[1];

	tap_dis_buff[0] = 0b00001111;  //tap azxes
	spi_transaction_t tap_dis = {
		.cmd = 0x2Au,
		.length = 8,
		.tx_buffer = tap_dis_buff
	};

	uint8_t int_src_buff[1];

	spi_transaction_t clear_int_src = {  //read interrupts source register to clear it
		.cmd = 0x80 | 0x40 | 0x30u,
		.length = 8,
		.rx_buffer = int_src_buff
	};

	uint8_t act_inact_ctl[1];
	//act_inact_ctl[0] = 0b11000000;     
	act_inact_ctl[0] = 0b11110000;     

	spi_transaction_t  adx_act_inact = {  // activate or deactivate axis interrupts
		.cmd = 0x27u,
		.length = 8,
		.tx_buffer = act_inact_ctl
	};

	spi_device_polling_transmit(spi, &adx_setbw);
	spi_device_polling_transmit(spi, &adx_dataformat);
	spi_device_polling_transmit(spi, &adx_treshhold);
	//spi_device_polling_transmit(spi, &adx_set_tresh_tap);
	//spi_device_polling_transmit(spi, &adx_set_dur);
	//spi_device_polling_transmit(spi, &tresh_read);
	//spi_device_polling_transmit(spi, &adx_inact_treshhold);
	//spi_device_polling_transmit(spi, &en1_readdata);
	spi_device_polling_transmit(spi, &adx_act_inact);
	//spi_device_polling_transmit(spi, &act_inact_read);
	//spi_device_polling_transmit(spi, &int_map_read);
	spi_device_polling_transmit(spi, &adx_int_map);
	spi_device_polling_transmit(spi, &clear_int_src);
	//spi_device_polling_transmit(spi, &tresh_inact_read);
	spi_device_polling_transmit(spi, &adx_int1_en);
	//spi_device_polling_transmit(spi, &tap_dis);

	/* printf("%d treshhold act\n", intbuff_r[0]);
	printf("%d treshhold inact\n", tresh_inact_buff[0]);
 	printf("%d interrupt en \n", en_int1buffR[0]);
	printf("%d interrupt map \n", int_map_buff_R[0]);
	printf("%d act inact \n", act_inact_read_buff[0]);  */
	spi_device_polling_transmit(spi, &adx_set_pow_ctl);
	//spi_device_polling_transmit(spi, &adx_read_bw);
	//printf("%d pw ctl \n", powctlbuff_r[0]);
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
	vTaskDelay(5 / portTICK_PERIOD_MS);



	gpio_config_t int_conf = {
		.pin_bit_mask = GPIO_SEL_34,
		.mode = GPIO_MODE_INPUT,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config_t int2_conf = {
		.pin_bit_mask = GPIO_SEL_35,
		.mode = GPIO_MODE_INPUT,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	gpio_config(&int_conf);
	gpio_config(&int2_conf);
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



	err = spi_bus_initialize(SPI3_HOST, &spi_bus_conf, 0);
	ESP_ERROR_CHECK(err);

	err = spi_bus_add_device(SPI3_HOST, &spi_device_conf, &spi);
	ESP_ERROR_CHECK(err);

	gpio_install_isr_service(0);

	gpio_isr_handler_add(SPI_INT1_PIN, spi_int_handler, (void *)SPI_INT1_PIN);
	gpio_isr_handler_add(SPI_INT2_PIN, spi_int2_handler, (void *)SPI_INT2_PIN);

	interruptQueue = xQueueCreate(10, sizeof(int));
	interrupt2Queue = xQueueCreate(10, sizeof(int));

	accel_init(spi);

	xTaskCreate(accel_interrupt, "accell 1 task", 2048, spi, 1, NULL);
//	xTaskCreate(beep, "beep  task", 2048, NULL, 2, NULL);
//	xTaskCreate(tap_interrupt, "accell 2 task", 2048, spi, 1, NULL);

	


}


