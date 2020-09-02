#include "ignite.h"

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


	return(ESP_OK);
}