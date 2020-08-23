#include "main.h"
#define EN_OLED GPIO_NUM_32
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define OLED_ADDR 0x3c
#define CHARGE_PUMP_VAL 0x33
#define CHARGE_PUMP_ON 0x32
#define CONTROL_ZERO 0x00
#define OLED_ON 0xAF
#define OLED_OFF 0xAE
#define OLED_FORCE_ON 0xA5


void create_load(uint8_t *arr, char *str, int len){
	int start = 0;
	int w = 7;
	int i = 0;
	int margin = (128 - strlen(str) * w) / 2;
	i = margin;
	for(int k = 0; k < len; k++){
		start = (str[k] - 32) * 6 + 0;
		w = 7;
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

void init(){

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
		printf("%s", "fuck");

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);



	err = i2c_master_write_byte(cmd, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
	if(err != ESP_OK){
		printf("%s", "fuck");
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


	if (err == ESP_OK) {
		ESP_LOGI("tag", "OLED configured successfully");
	} else {
		ESP_LOGE("tag", "OLED configuration failed. code: 0x%.2X", err);
	}

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

void clear_all(){
	uint8_t buff[8][128];

 	for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  buff[y][x] = 0b00000000;
			  write_page(&buff[y][x], y);
            

        }
    }
}

void app_main(void){


	gpio_set_direction(EN_OLED, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_OLED, 1);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	
	init();
	clear_all();
 
	uint8_t arr[8][128];


	for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			 arr[y][x] = 0b00000000;
        }
    }

	char *str = "Hello World!";

	for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  arr[y][x] = 0b00000000;
        }
    }
	char bunchs[8][18];

	 for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 18; x++) {
			  bunchs[y][x] = 0x00;
        }
    }

	int i = 0;
	int count = 0;
	int j = 0;

	while(i < 8)	{
		if(count >= strlen(str)){
			break;
		}
		j = 0;
		while(j < 17){
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
	
		create_load(arr[y], bunchs[y], strlen(bunchs[y]));

    }


//	int y = 0;
//	while(y < 8){
	for (uint8_t x = 0; x < 128; x++) {
			write_page(&arr[0][x], 3);
			vTaskDelay(20 / portTICK_PERIOD_MS);
    }
//		y++;
//	}


}


