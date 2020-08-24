#include "main.h"
#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13




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
		.max_transfer_sz = 32,
/* 		.flags=,
		.intr_flags=, */

	};


	spi_device_interface_config_t spi_device_conf = {
/* 		.command_bits =,
		.address_bits =,
		.dummy_bits =, */
		.mode = 0,
		.duty_cycle_pos = 0,
	//	.cs_ena_pretrans=,
	//	.cs_ena_posttrans=,
		.clock_speed_hz = 26*1000*1000,
		.input_delay_ns = 0,
		.spics_io_num = SPI_MISO,
		//.flags=,
		.queue_size = 7,

	};

	uint8_t buff[6];
	uint8_t status;

 	spi_transaction_t spi_transaction_conf = {
	//	.flags=,
		//.cmd= {0b11111111, 0b00000000, },
		//.addr=,
		.length = 32,
		.rxlength = 16,
		.rx_buffer = &buff,
	};

	spi_transaction_t spi_transaction_conf_start = {
	//	.flags=,
		.cmd = 0b00000000,
		//.addr=,
	//	.length = 32,
	//	.rxlength = 16,
	//	.rx_buffer = &buff,
	};

	spi_transaction_t spi_transaction_conf_status = {
	//	.flags=,
		.cmd = 0x27,
		//.addr=,
	//	.length = 32,
	//	.rxlength = 16,
		.rx_buffer = &status,
	};

	spi_transaction_t spi_transaction_conf_xl = {
	//	.flags=,
		.cmd = 0x28,
		//.addr=,
	//	.length = 32,
	//	.rxlength = 16,
		.rx_buffer = &buff[0],
	};

	spi_transaction_t spi_transaction_conf_xh = {
	//	.flags=,
		.cmd = 0x29,
		//.addr=,
	//	.length = 32,
	//	.rxlength = 16,
		.rx_buffer = &buff[1],
	};

	spi_transaction_t spi_transaction_conf_yl = {
	//	.flags=,
		.cmd = 0x2A,
		//.addr=,
	//	.length = 32,
	//	.rxlength = 16,
		.rx_buffer = &buff[2],
	};

	spi_transaction_t spi_transaction_conf_yh = {
	//	.flags=,
		.cmd = 0x2B,
		//.addr=,
	//	.length = 32,
	//	.rxlength = 16,
		.rx_buffer = &buff[3],
	};

/* 	spi_transaction_t spi_rec_transaction_conf = {
	//	.flags=,
		//.cmd= {0b11111111, 0b00000000, },
		//.addr=,
		.length = 32,
		.rxlength = 16,
		.tx_buffer =,
		.rx_buffer
	};   */

/* 	1. Write CTRL_REG1
2. Write CTRL_REG2
3. Write CTRL_REG3
4. Write CTRL_REG4
5. Write CTRL_REG5
6. Write CTRL_REG6
7. Write Reference
8. Write INT1_THS
9. Write INT1_DUR
10. Write INT1_CFG
11. Write CTRL_REG5
	 */

	err = spi_bus_initialize(SPI2_HOST, &spi_bus_conf, 0);
	ESP_ERROR_CHECK(err);

	err = spi_bus_add_device(SPI2_HOST, &spi_device_conf, &spi);
	ESP_ERROR_CHECK(err);

/* 	spi_device_queue_trans(spi, &spi_transaction_conf, portMAX_DELAY);

	spi_device_get_trans_result(spi, spi_transaction_conf, portMAX_DELAY); */

	//spi_device_polling_start(spi, &spi_transaction_conf, portMAX_DELAY);

	//spi_device_polling_transmit(spi, &spi_transaction_conf_start);

	//spi_device_polling_transmit(spi, &spi_transaction_conf);

	//spi_device_polling_transmit(spi, &spi_transaction_conf);

	//spi_device_polling_end(spi, portMAX_DELAY);

	


printf("%p", spi);

	while (true)
	{
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		spi_device_polling_transmit(spi, &spi_transaction_conf_start);

		//vTaskDelay(2000 / portTICK_PERIOD_MS);

		spi_device_polling_transmit(spi, &spi_transaction_conf);

		

		spi_device_polling_transmit(spi, &spi_transaction_conf_status);


		spi_device_polling_transmit(spi, &spi_transaction_conf_xl);
		spi_device_polling_transmit(spi, &spi_transaction_conf_xh);
		spi_device_polling_transmit(spi, &spi_transaction_conf_yl);
		spi_device_polling_transmit(spi, &spi_transaction_conf_yh);

		printf("%s\n", "----------");
		printf("%x status \n", status);

			

		for(int i = 0; i < 6; i++){
			printf("%x\n", buff[i]);
		}

	}

}


