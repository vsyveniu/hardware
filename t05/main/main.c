#include "main.h"
#define UART TX 17
#define UART RX 16




void app_main(void)
{
	uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
	};	

	//uart_driver_install(UART_NUM_2, 1024 * 2, 0, 0, NULL, 0);

	ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_TX, UART_RX))
	 while (true){

		
	 }
}


