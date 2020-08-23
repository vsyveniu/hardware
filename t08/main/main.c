#include "main.h"
#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13




void app_main(void){
	gpio_set_direction(EN_ACCEL, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_ACCEL, 1);

	spi_bus_config_t spi_bus_conf = {

	};

	spi_transaction_t spi_transaction_conf = {

	};



	while (true)
	{

	}

}


