#include "main.h"
#define EN_ACCEL GPIO_NUM_23
#define SPI_CLK GPIO_NUM_14
#define SPI_MISO GPIO_NUM_12
#define SPI_MOSI GPIO_NUM_13
#define SPI_CS GPIO_NUM_15
#define SPI_INT1_PIN GPIO_NUM_34
#define EN_AMP 5

#define EN_OLED GPIO_NUM_32
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define OLED_ADDR 0x3c


#define DHT_PIN GPIO_NUM_4
#define DHT_POWER GPIO_NUM_2
#define DHT_RES_TYPE_TEMPERATURE 2
#define DHT_RES_TYPE_HUMIDITY 0


static char str_buff[16];

int get_DHT_data(int type){
	
  u_int8_t dataBytes[5];
  int count = 0;
  int byteNum = 0;
  int bitNum = 7;
  int bitCount = 0;


  for (int i = 0; i <5; i++)
  		dataBytes[i] = 0; 

  	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_level(DHT_PIN, 0); 
	vTaskDelay(20 / portTICK_PERIOD_MS);
	gpio_set_level(DHT_PIN, 1);

	count = 0;
	while(gpio_get_level(DHT_PIN) != 0){
		gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
		if(count > 45){
			return ESP_ERR_TIMEOUT;
		}
		if(gpio_get_level(DHT_PIN) == 0){
			break;
		}
		count++;
		ets_delay_us(1);
	}
	count = 0;
	while(gpio_get_level(DHT_PIN) == 0)
	{
		if(count > 80)
		{
			return ESP_ERR_TIMEOUT;
		}
		count++;
		ets_delay_us(1);
	}
	count = 0;
	while(gpio_get_level(DHT_PIN) == 1)
	{
		if(count > 80)
		{	
			 fflush (stdout);
			return ESP_ERR_TIMEOUT;
		}
		if(gpio_get_level(DHT_PIN) == 0)
			break;
		count++;
		ets_delay_us(1);
	}

	bitCount = 0;
	while(bitCount < 40){
	
		count = 0;
		while(gpio_get_level(DHT_PIN) == 0)
		{
			if (count > 50)
			{
				return ESP_ERR_TIMEOUT;
			}
			if(gpio_get_level(DHT_PIN) == 1)
				break;
			count++;
			ets_delay_us(1);
		}
		count = 0;
		while(gpio_get_level(DHT_PIN) == 1)
		{
			if (count > 70)
			{
				return ESP_ERR_TIMEOUT;
			}
			if(gpio_get_level(DHT_PIN) == 0)
				break;
			count++;
			ets_delay_us(1);
		}
  	  
		if (count > 28)
			dataBytes[byteNum] |= (1 << bitNum);
			
		if(bitNum == 0)
		{
			byteNum += 1;
			bitNum = 7;
		}
		else
			bitNum -= 1;
		
		bitCount++;
	}
		int checksum = dataBytes[0] + dataBytes[2] + dataBytes[3];

	if(checksum != dataBytes[4])
		return (-1);



	
	return (dataBytes[type]);
}


/////  START OLED

int init_oled(){

	esp_err_t err;
	i2c_cmd_handle_t cmd;
	i2c_config_t i2c_conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_SDA,
		.scl_io_num = I2C_SCL,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 1000000,
	};
	err = i2c_param_config(I2C_NUM_0, &i2c_conf);

	err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	if(err != ESP_OK)
		return (ESP_FAIL);

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);



	err = i2c_master_write_byte(cmd, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
	if(err != ESP_OK){
		return (ESP_FAIL);
	}

	i2c_master_write_byte(cmd, 0x00, true); // command stream
    i2c_master_write_byte(cmd, 0xAE, true); // off
    i2c_master_write_byte(cmd, 0xD5, true); // clock div
    i2c_master_write_byte(cmd, 0x80, true);
    i2c_master_write_byte(cmd, 0xA8, true); // multiplex
    i2c_master_write_byte(cmd, 0xFF, true);
    i2c_master_write_byte(cmd, 0x8D, true); // charge pump
    i2c_master_write_byte(cmd, 0x14, true);
    i2c_master_write_byte(cmd, 0x10, true); // high column
    i2c_master_write_byte(cmd, 0xB0, true);
    i2c_master_write_byte(cmd, 0xC8, true);  //reverse page order (from up to down)
    i2c_master_write_byte(cmd, 0x00, true); // low column
    i2c_master_write_byte(cmd, 0x10, true);
    i2c_master_write_byte(cmd, 0x40, true);
    i2c_master_write_byte(cmd, 0xA1, true); // segment remap
    i2c_master_write_byte(cmd, 0xA6, true);
    i2c_master_write_byte(cmd, 0x81, true); // contrast
    i2c_master_write_byte(cmd, 0xFF, true);
    i2c_master_write_byte(cmd, 0xAF, true); // on0xA1
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);


	if (err != ESP_OK)
		return (ESP_FAIL);

	return (ESP_OK);

}


void write_page(uint8_t *data, uint8_t page) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x80, true); // single command
    i2c_master_write_byte(cmd, 0xB0 + page, true);
    i2c_master_write_byte(cmd, 0x40, true); // data stream
    i2c_master_write(cmd, data, 1, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}


void create_load(uint8_t *arr, char *str, int len, int font_weight){
	int start = 0;
	int w = font_weight;
	int i = 0;
	int margin = (128 - strlen(str) * w) / 2;
	i = margin;
	for(int k = 0; k < len; k++){
		start = (str[k] - 32) * 6 + 0;
		w = font_weight;
		while (i < 128){
			if(w == 0){
					break;
				}
				if(w <= 1){
					arr[i] = 0x00;
				}
				arr[i] = font6x8[start];
				start++;
				w--;
				i++;
		}		
	}	
}

void display_str(char *str, int page, int appear_speed, int font_weight){


	uint8_t arr[8][128];
	int i = 0;
	int j = 0;
	int count = 0;
	int chars_len;

	chars_len = 128 / font_weight;  //6 for longer words(no 1 bit margin between chars and 128/6 = 20 chars will fit in row), 7 for shorter(1 bit margin between chars). Other values will screw up entire string
	char bunchs[8][chars_len];

	for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  arr[y][x] = 0b00000000;
        }
    }

	printf("%s", str);

	 for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < chars_len; x++) {
			  bunchs[y][x] = 0x00;
        }
    }



	while(i < 8)	{
		if(count >= strlen(str)){
			break;
		}
		j = 0;
		while(j < chars_len){
			bunchs[i][j] = str[count];
			count++;
			if(count >= strlen(str)){
				break;
			}
			j++;
		}

		i++;
	}

	for (uint8_t y = 0; y < 8; y++) {
	
		create_load(arr[y], bunchs[y], strlen(bunchs[y]), font_weight);

    }

	for (uint8_t x = 0; x < 128; x++) {
			write_page(&arr[0][x], page);
			vTaskDelay(appear_speed / portTICK_PERIOD_MS);
    }


}

void clear_oled(){
	uint8_t buff[8][128];

 	for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  buff[y][x] = 0b00000000;
			  write_page(&buff[y][x], y);
            

        }
    }
}



//// END OLED


//// ACCELL START

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

	spi_device_polling_transmit(spi, &adx_setbw);
	spi_device_polling_transmit(spi, &adx_dataformat);
	spi_device_polling_transmit(spi, &adx_treshhold);
	spi_device_polling_transmit(spi, &adx_act_inact);
	spi_device_polling_transmit(spi, &adx_int_map);
	spi_device_polling_transmit(spi, &adx_int1_en);
	spi_device_polling_transmit(spi, &adx_set_pow_ctl);

}
/// ACCELL END

int ignite_parts(){

	esp_err_t err;
	spi_device_handle_t spi;

	gpio_set_direction(DHT_POWER, GPIO_MODE_OUTPUT);	
	gpio_set_level(DHT_POWER, 1);

	gpio_set_direction(EN_OLED, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_OLED, 1);

	gpio_set_direction(EN_ACCEL, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_ACCEL, 1);

	gpio_set_direction(EN_AMP, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_AMP, 1);

	vTaskDelay(50 / portTICK_PERIOD_MS);
	if(init_oled() != ESP_OK){  //init oled
		return (ESP_FAIL);
	}
	clear_oled();
	display_str("Ignition...", 3, 10, 7);
	vTaskDelay(3000 / portTICK_PERIOD_MS); // delay for DHT11 ready

	/// Accelerometer initialization

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


	/// END Accelerometer initialization

	return(ESP_OK);
}

void app_main(void){

	int temperature = 0;

	//static char str_buff[16];
	//memset(str_buff, 0, strlen(str_buff));

	if(ignite_parts() != ESP_OK)
		printf("Something broken!");

	clear_oled();
	display_str("Done!", 3, 10, 7);

	temperature = get_DHT_data(DHT_RES_TYPE_TEMPERATURE);

	sprintf(str_buff, "Temperature %dC", temperature);
	clear_oled();
	display_str(str_buff, 3, 10, 7);

	//display_str();





	

	//prepare_dht();
	//display_str("Preparing");


	int humidity = 0;
	while(true){
 		vTaskDelay(3000 / portTICK_PERIOD_MS);
	 	temperature = get_DHT_data(DHT_RES_TYPE_TEMPERATURE);
		 sprintf(str_buff, "Temperature %dC", temperature);
		clear_oled();
		display_str(str_buff, 3, 10, 7);
		vTaskDelay(3000 / portTICK_PERIOD_MS);
		humidity = get_DHT_data(DHT_RES_TYPE_HUMIDITY);
	printf("%d te\n", temperature);
	printf("%d hu\n", humidity);
	vTaskDelay(3000 / portTICK_PERIOD_MS); 
	 
	}
}


