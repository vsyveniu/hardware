#include "main.h"
#include "defines.h"

static char temperature_buff[16];
static int temperature = 0;
static char humidity_buff[15];
static int humidity = 0;
static char oled_page = OLED_PAGE_TEMPERATURE;
static char prev_page = OLED_PAGE_TEMPERATURE;
static int oled_orientation = OLED_NORMAL;
static int oled_orientation_prev = OLED_NORMAL;

xQueueHandle button1Queue;
xQueueHandle button2Queue;
xQueueHandle accelQueue;

static void IRAM_ATTR butt1_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(button1Queue, &pin, NULL);
}

static void IRAM_ATTR butt2_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(button2Queue, &pin, NULL);
}

static void IRAM_ATTR spi_int_handler(void *args){

	uint32_t pin = (uint32_t) args;
	xQueueSendFromISR(accelQueue, &pin, NULL);
}

void butt1_pushed()
{
	uint32_t pin;
	int count;

	while (true){
		if(xQueueReceive(button1Queue, &pin, portMAX_DELAY)){
			gpio_intr_disable(pin);

			oled_page = OLED_PAGE_TEMPERATURE;
			beep();

			count = 0;

			while(gpio_get_level(pin) != 1 && count < 10){

				if(gpio_get_level(pin) == 0){
					vTaskDelay(10/portTICK_PERIOD_MS);
					count = 0;
				}
				else if(gpio_get_level(pin) == 1){
					count++;
				}				
			}
			gpio_intr_enable(pin);	
		}
	}
}

void butt2_pushed()
{
	uint32_t pin;
	int count;

	while (true){
		
		if(xQueueReceive(button2Queue, &pin, portMAX_DELAY))
		{
			gpio_intr_disable(pin);

			oled_page = OLED_PAGE_HUMIDITY;
			beep();
			count = 0;

			while(gpio_get_level(pin) != 1 && count < 10){

				if(gpio_get_level(pin) == 0){
					vTaskDelay(10/portTICK_PERIOD_MS);
					count = 0;
				}
				else if(gpio_get_level(pin) == 1){
					count++;
				}				
			}
			gpio_intr_enable(pin);	
		}
	}
}

void accel_flipped(void *spi){
	
	uint8_t pin;
	double y = 0;

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
		
		if(xQueueReceive(accelQueue, &pin, portMAX_DELAY))
		{
			spi_device_polling_transmit(spi, &adx_readData); 
			spi_device_polling_transmit(spi, &int_src);  // after reading data should read interrupt source register to clear it for next interrupts can be fired

			y = (double)buff[1]/256.0;

			if (y > 1.05){
				oled_orientation = OLED_REVERSE;
			}
			else if (y < -0.9){
				oled_orientation = OLED_NORMAL;	
			}
		}
	}
}

void oled_reverse(int orientation){

	i2c_cmd_handle_t cmd;
	int oled_orientation = 0xC8;
	int page_order = 0xA1;

	(orientation == OLED_REVERSE) ? oled_orientation = 0xC0 : 0;
	(orientation == OLED_REVERSE) ? page_order = 0xA0 : 0;
	
	clear_oled();

	cmd = i2c_cmd_link_create();

   	i2c_master_start(cmd);
   
    i2c_master_write_byte(cmd, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, 0x00, true); 
	i2c_master_write_byte(cmd, page_order, true);  
    i2c_master_write_byte(cmd, oled_orientation, true);  
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

	display_str("........", 3, 10, 7);
}

void measure(){
	uint8_t dht_data[5];
	vTaskDelay(2000 / portTICK_PERIOD_MS); // delay for DHT11 ready

	while(true){
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		if(get_DHT_data(dht_data) == ESP_OK){
			temperature = dht_data[2];
			humidity = dht_data[0];
			oled_page = prev_page;
		}
		else{
			oled_page = DHT_RETARD;
			clear_oled();
		}
	}
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
		spi_device_polling_transmit(spi, &int_src);
		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
}

int ignite_parts(){

	esp_err_t err;
	spi_device_handle_t spi;

	gpio_set_direction(EN_OLED, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_OLED, 1);
	
	gpio_set_direction(DHT_POWER, GPIO_MODE_OUTPUT);	
	gpio_set_level(DHT_POWER, 1);

	gpio_set_direction(EN_AMP, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_AMP, 1);
	dac_output_enable(DAC_CHANNEL_1);

	gpio_set_direction(EN_ACCEL, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_ACCEL, 1);
	vTaskDelay(1000/portTICK_PERIOD_MS);
	
	if(init_oled() != ESP_OK){  
		return (ESP_FAIL);
	}
	clear_oled();
	display_str("Ignition...", 3, 10, 7);

	gpio_config_t butt_1_conf = {
		.pin_bit_mask = GPIO_SEL_39,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};
	gpio_config_t butt_2_conf = {
		.pin_bit_mask = GPIO_SEL_18,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	err = gpio_config(&butt_1_conf);
	if(err != ESP_OK){
		display_str("Gpio 39 broken!", 3, 10, 6);
		return (ESP_FAIL);
	}

	err = gpio_config(&butt_2_conf);
	if(err != ESP_OK){
		display_str("Gpio 18 broken!", 3, 10, 6);
		return (ESP_FAIL);
	}

	err = gpio_install_isr_service(0);
	if(err != ESP_OK){
		display_str("ISR service broken!", 3, 10, 6);
		return (ESP_FAIL);
	}

	gpio_isr_handler_add(BUTT_1, butt1_handler, (void *)BUTT_1);
	gpio_isr_handler_add(BUTT_2, butt2_handler, (void *)BUTT_2);
	gpio_isr_handler_add(SPI_INT1_PIN, spi_int_handler, (void *)SPI_INT1_PIN);

	gpio_intr_disable(BUTT_1);
	gpio_intr_disable(BUTT_2);
	gpio_intr_disable(SPI_INT1_PIN);

	button1Queue  = xQueueCreate(10, sizeof(int));
	button2Queue = xQueueCreate(10, sizeof(int));
	accelQueue = xQueueCreate(10, sizeof(int));

		gpio_config_t accel_int_conf = {
		.pin_bit_mask = GPIO_SEL_34,
		.mode = GPIO_MODE_INPUT,
		.intr_type = GPIO_INTR_POSEDGE,
	};

	err = gpio_config(&accel_int_conf);
	if(err != ESP_OK){
		display_str("Gpio 34 broken!", 3, 10, 6);
		return (ESP_FAIL);
	}

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
		.queue_size = 5,
	};

	err = spi_bus_initialize(SPI3_HOST, &spi_bus_conf, 0);
	if(err != ESP_OK){
		display_str("Accelerometer broken!", 3, 10, 6);
		return (ESP_FAIL);
	}

	err = spi_bus_add_device(SPI3_HOST, &spi_device_conf, &spi);
	if(err != ESP_OK){
		display_str("Accelerometer broken!", 3, 10, 6);
		return (ESP_FAIL);
	}

	accel_init(spi);

	xTaskCreatePinnedToCore(butt1_pushed, "button 1 task", 2048, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(butt2_pushed, "button 2 task", 2048, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(accel_flipped, "accelerometer task", 2048, spi, 1, NULL, 1);
	xTaskCreatePinnedToCore(accel_keep_alive_crutch, "accelerometer crutch task", 2048, spi, 1, NULL, 1);
	xTaskCreate(measure, "measure task", 2048, NULL, 1, NULL);

	return(ESP_OK);
}

void app_main(void){
	
	uint8_t dht_data[5];
	if(ignite_parts() != ESP_OK){
		display_str("Have to reboot!", 3, 0, 6);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		esp_restart();
	}

	get_DHT_data(dht_data);
	temperature = dht_data[2];
	humidity = dht_data[0];
	clear_oled();
	display_str("Done!", 3, 10, 7);

	gpio_intr_enable(BUTT_1);
	gpio_intr_enable(BUTT_2);
	gpio_intr_enable(SPI_INT1_PIN);

	sprintf(temperature_buff, "Temperature %dC", temperature);
	
	clear_oled();
	display_str(temperature_buff, 3, 10, 7);

	while(true){

		if(oled_orientation != oled_orientation_prev){
			oled_orientation_prev = oled_orientation;
			oled_reverse(oled_orientation);
		}
		if(oled_page == 0){
			if(temperature != 0){
			sprintf(temperature_buff, "Temperature %dC", temperature);
			display_str(temperature_buff, 3, 1, 7);
			prev_page = OLED_PAGE_TEMPERATURE;
			}
			else{
				display_str("You hurt me!", 3, 1, 7);
			}
		}
		else if(oled_page == 1){
			if(humidity != 0){
			sprintf(humidity_buff, "Humidity %d%%", humidity);
			display_str(humidity_buff, 3, 1, 7);	
			prev_page = OLED_PAGE_HUMIDITY;
			}
			else{
				display_str("You hurt me!", 3, 1, 7);
			}
		}
		else if(oled_page == DHT_RETARD){
			clear_oled();
			display_str("Wait for dht...", 3, 1, 7);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
