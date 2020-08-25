#include "main.h"
#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13
#define SPI_CS GPIO_NUM_15
#define SPI_INT1_PIN GPIO_NUM_34




void app_main(void){

	esp_err_t err;

	gpio_set_direction(EN_ACCEL, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_ACCEL, 1);
	vTaskDelay(5 / portTICK_PERIOD_MS);

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

	//buff[0] = 0x00;
	for(int i = 0; i < 8; i++){
		buff[i] = 0x00;
	}



 	spi_transaction_t spi_transaction_conf = {
		.flags = 0x80u,
		//.cmd = 0xff,
		.addr= 0x00,
		.length = 8 * 8,
		.rxlength = 0,
		.tx_buffer = &buff,
		.rx_buffer = &buff,
	};


	spi_transaction_t spi_start = {
		.flags = SPI_TRANS_USE_RXDATA,
		.cmd = 0x00,
		.length = 8,
	};

	spi_transaction_t spi_start2 = {
		.flags = SPI_TRANS_USE_RXDATA,
		.cmd = 0x2C,
		.length = 8,
	};
 
	int16_t wbuff[3];
	
 	spi_transaction_t spi_write = {
		.flags = SPI_TRANS_USE_RXDATA,
		.cmd = 0x2Du,
		.tx_buffer = wbuff,
		.length = 8,
	};  

	int16_t bufftx[3];
	spi_transaction_t spi_readData = {
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





	
	while (true)
	{
		vTaskDelay(200 / portTICK_PERIOD_MS);
		//spi_device_polling_transmit(spi, &spi_transaction_conf_start);

		//vTaskDelay(2000 / portTICK_PERIOD_MS);

		spi_device_polling_transmit(spi, &spi_start);
		spi_device_polling_transmit(spi, &spi_start2);
		spi_device_polling_transmit(spi, &spi_write);

		
		spi_device_polling_transmit(spi, &spi_readData);





		

		//spi_device_polling_transmit(spi, &spi_transaction_conf_status);


		//spi_device_polling_transmit(spi, &spi_transaction_conf_xl);
		//spi_device_polling_transmit(spi, &spi_transaction_conf_xh);
		//spi_device_polling_transmit(spi, &spi_transaction_conf_yl);
		//spi_device_polling_transmit(spi, &spi_transaction_conf_yh);

		printf("%s\n", "----------");
		//printf("%x status \n", status);


			
		//printf("%X start 1 \n", spi_start.rx_data[0]);

	//	printf("%X start 2 \n", spi_start2.rx_data[0]);
	/* 	for(int i = 0; i < 3; i++){
			printf("%d\n", buff[i]);
		} */


		printf("%d x\n ", buff[0]);
		printf("%d y\n ", buff[1]);
		printf("%d z\n ", buff[2]);

		printf("%d GYD\n ", a);


	}

}


