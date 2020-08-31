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

#define OLED_PAGE_TEMPERATURE 0
#define OLED_PAGE_HUMIDITY 1
#define DHT_RETARD 2

#define BUTT_1 	GPIO_NUM_39
#define BUTT_2 	GPIO_NUM_18


static char temperature_buff[16];
static int temperature = 0;
static char humidity_buff[15];
static int humidity = 0;
static char oled_page = OLED_PAGE_TEMPERATURE;
static char prev_page = OLED_PAGE_TEMPERATURE;

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

void beep(){

int volt;
int beep_count;

		 for (beep_count = 0; beep_count < 300; beep_count++){ 

              for (volt = 0; volt < 256; volt+=40)
                { 
                    dac_output_voltage(DAC_CHANNEL_1, volt);
                
                }     
                ets_delay_us(1);
            }

            for (beep_count = 300; beep_count > 0; beep_count--){ 
				for (volt = 256; volt  > 0; volt-=15)
                    {
                        dac_output_voltage(DAC_CHANNEL_1, volt);
                    
                    }      
                ets_delay_us(1);
            }
}



int get_DHT_data(uint8_t *dataBytes){
	
  //u_int8_t dataBytes[6];
  int count = 0;
  int byteNum = 0;
  int bitNum = 7;
  int bitCount = 0;


  for (int i = 0; i <6; i++)
  		dataBytes[i] = 0; 

  	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_level(DHT_PIN, 0); 
	vTaskDelay(20 / portTICK_PERIOD_MS);
	gpio_set_level(DHT_PIN, 1);

	count = 0;
	while(gpio_get_level(DHT_PIN) != 0){
		gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
		if(count > 45){
			return (ESP_ERR_TIMEOUT);
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
			return (ESP_ERR_TIMEOUT);
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
			return (ESP_ERR_TIMEOUT);
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
				return (ESP_ERR_TIMEOUT);
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
				return (ESP_ERR_TIMEOUT);
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
		return (ESP_ERR_INVALID_CRC);

	return (ESP_OK);
}


/////  START OLED

int init_oled(oled_settings_t *oled_conf){

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
    i2c_master_write_byte(cmd, oled_conf->page_orientation, true);  //reverse page order (from up to down)
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


//// interrupts

void butt1_pushed()
{
	uint32_t pin;
	int count;

	while (true)
	{
		
		if(xQueueReceive(button1Queue, &pin, portMAX_DELAY))
		{

			gpio_intr_disable(pin);

			oled_page = OLED_PAGE_TEMPERATURE;
			beep();

			count = 0;

			while(gpio_get_level(pin) != 1 && count > 0){
				vTaskDelay(20 / portTICK_PERIOD_MS);
				if(gpio_get_level(pin) == 0)
					count++;
				else
					vTaskDelay(20 / portTICK_PERIOD_MS);
			}
			gpio_intr_enable(pin);
			
		}
	}
}

void butt2_pushed()
{
	uint32_t pin;
	int count;

	while (true)
	{
		
		if(xQueueReceive(button2Queue, &pin, portMAX_DELAY))
		{

			gpio_intr_disable(pin);

			oled_page = OLED_PAGE_HUMIDITY;
			beep();
			count = 0;

			while(gpio_get_level(pin) != 1 && count > 0){
				vTaskDelay(20 / portTICK_PERIOD_MS);
				if(gpio_get_level(pin) == 0)
					count++;
				else
					vTaskDelay(20 / portTICK_PERIOD_MS);
			}

			gpio_intr_enable(pin);
			
		}
	}
}

void accel_flipped(void *spi){
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
		
		if(xQueueReceive(accelQueue, &pin, portMAX_DELAY))
		{
			
			printf("%s z \n", "fuck interrupt");

			spi_device_polling_transmit(spi, &adx_readData);
			spi_device_polling_transmit(spi, &int_src);

			x = (double)buff[0]/256.0;
			y = (double)buff[1]/256.0;
			z = (double)buff[2]/256.0;

			is_x_low = (fabs(x) < 0.15);
    	    is_y_low = (fabs(y) < 0.15);
    	    is_z_low = (fabs(z) < 0.15);
			

			printf("%d\n", is_x_low);
			printf("%d\n", is_y_low);
			printf("%d\n", is_z_low);

			if (is_z_low && is_y_low && !z_fired && !is_x_low)
    		{
				printf("%s\n", "sdf");

				z_fired = 1;
				y_fired = 0;
				x_lock = 1;
				y_lock = 1;
				printf("%d z\n", z_fired);
				beep();

				count = 0;
			}
			else if(!is_z_low && is_y_low && !is_x_low )
			{

				z_fired = 0;
				y_fired = 0;
				printf("%d z\n", z_fired);

				count = 0;
				z_lock = 0;

			}
	
			else if (is_z_low && is_x_low && !x_fired && !is_y_low)
    		{
				printf("%s\n", "ggggdf");

				x_fired = 1;
				printf("%d z\n", z_fired);
				beep();
				count = 0;
			}
			else if(!is_z_low && is_x_low && !is_y_low)
			{
	
				x_fired = 0;

				printf("%d z\n", z_fired);

				count = 0;
				z_lock = 0;
			}

			else if ((is_z_low && !is_x_low && !y_fired))
    		{
				printf("%s\n", "sddddf");
				
		
				y_fired = 1;
				printf("%d z\n", z_fired);
				beep();

				count = 0;
			}
			else if((!is_z_low && !is_x_low && !is_y_low))
			{
	
				y_fired = 0;
				printf("%d z\n", z_fired);

				count = 0;
				z_lock = 0;
			}

		}
	}
}

/// END INTERRUTPS
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
	dac_output_enable(DAC_CHANNEL_1);

	oled_settings_t oled_conf = {
		.page_orientation = 0xC8,
	};


	vTaskDelay(50 / portTICK_PERIOD_MS);
	if(init_oled(&oled_conf) != ESP_OK){  //init oled
		return (ESP_FAIL);
	}
	clear_oled();
	display_str("Ignition...", 3, 10, 7);
	
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


	xTaskCreatePinnedToCore(butt1_pushed, "button 1 task", 2048, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(butt2_pushed, "button 2 task", 2048, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(accel_flipped, "accelerometer task", 2048, spi, 1, NULL, 1);


	return(ESP_OK);
}

void measure(){
	uint8_t dht_data[5];

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

void app_main(void){
	
	uint8_t dht_data[5];

	vTaskDelay(5 / portTICK_PERIOD_MS);


	if(ignite_parts() != ESP_OK){
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		display_str("Have to reboot!", 3, 0, 6);
		printf("Something broken!");
	}

	vTaskDelay(2000 / portTICK_PERIOD_MS); // delay for DHT11 ready	

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

	xTaskCreate(measure, "measure task", 2048, NULL, 1, NULL);
	
	
	while(true){
		
		if(oled_page == 0){
			vTaskDelay(10 / portTICK_PERIOD_MS);
		 	sprintf(temperature_buff, "Temperature %dC", temperature);
			display_str(temperature_buff, 3, 1, 7);
			prev_page = OLED_PAGE_TEMPERATURE;
		}
		else if(oled_page == 1)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			sprintf(humidity_buff, "Humidity %d%%", humidity);
			display_str(humidity_buff, 3, 1, 7);	
			prev_page = OLED_PAGE_HUMIDITY;
		}
		else if(oled_page == DHT_RETARD){
			vTaskDelay(10 / portTICK_PERIOD_MS);
			display_str("Wait for dht...", 3, 1, 7);
		}
	}
}


