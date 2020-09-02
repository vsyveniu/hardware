#include "accelerometer.h"

void accel_init(spi_device_handle_t spi){

	int8_t adx_setbw_buff[1];
	adx_setbw_buff[0] = 0b00001010;   // set bw rate to 100hz output

	spi_transaction_t adx_setbw = {
		.cmd = 0x2Cu,
		.length = 8,
		.tx_buffer = adx_setbw_buff
	};
 
	uint8_t powctlbuff[1];
	powctlbuff[0] = 0b0001100;  // set link,measure,sleep,wakeup stuff. We need measurement mode

	spi_transaction_t adx_set_pow_ctl = {
		.cmd = 0x2Du,
		.tx_buffer = powctlbuff,
		.length = 8,
	};

	uint8_t tresh_buff[1];
	tresh_buff[0] = 2;  //set tresh_act register. When activity is detected, the magnitude compared with this value. If magnitude exceeds a value, interrupt is fired

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
	act_inact_ctl[0] = 0b11000000;    // activate or deactivate axis interrupts

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

	spi_device_polling_transmit(spi, &adx_set_pow_ctl);
	spi_device_polling_transmit(spi, &adx_setbw);
	spi_device_polling_transmit(spi, &adx_dataformat);
	spi_device_polling_transmit(spi, &adx_treshhold);
	spi_device_polling_transmit(spi, &adx_act_inact);
	spi_device_polling_transmit(spi, &adx_int_map);
	spi_device_polling_transmit(spi, &adx_int1_en);
	spi_device_polling_transmit(spi, &clear_int_src);

}


