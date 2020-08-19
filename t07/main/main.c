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

void sh1106_write_page(sh1106_t *display, uint8_t page) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (display->addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x80, true); // single command
    i2c_master_write_byte(cmd, 0xB0 + page, true);
    i2c_master_write_byte(cmd, 0x40, true); // data stream
    i2c_master_write(cmd, display->pages[page], 128, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(display->port, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void sh1106_set(sh1106_t *display, uint8_t x, uint8_t y, bool s) {
    const uint8_t i = y / 8;
    if (s) {
        display->pages[i][x] |= (1 << (y % 8));
    } else {
        display->pages[i][x] &= ~(1 << (y % 8));
    }
    display->changes |= (1 << i);
}

void sh1106_update(sh1106_t *display) {
    for (uint8_t i = 0; i < 16; i++) {
        if (display->changes & (1 << i)) {
            sh1106_write_page(display, i);
        }
    }
    display->changes = 0x0000;
}

void get_char(){
	int w = 6;
	while(w > 0){
		printf("%x", font6x8[34]);
	}

	
	//int charmap[6] =
	//	y * w + x
}


void app_main(void){

	get_char();

	esp_err_t err;
	i2c_cmd_handle_t cmd;

	gpio_set_direction(EN_OLED, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_OLED, 1);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

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






	//i2c_master_stop(cmd);

//	err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1/portTICK_PERIOD_MS);
	if (err == ESP_OK) {
		ESP_LOGI("tag", "OLED configured successfully");
	} else {
		ESP_LOGE("tag", "OLED configuration failed. code: 0x%.2X", err);
	}
//	i2c_cmd_link_delete(cmd);


/* for(uint8_t x = 0; x < 128; x++){
	for(uint8_t y = 0; y < 128; y++){ */
/* 
	for(uint8_t p=0;p<18;p++){
		i2c_cmd_handle_t cmd2 = i2c_cmd_link_create();
		i2c_master_start(cmd2);
		i2c_master_write_byte(cmd2, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
		i2c_master_write_byte(cmd2, 0x80, true); // single command
		i2c_master_write_byte(cmd2, 0xB0 | p, true); // single command
		i2c_master_write_byte(cmd2, 0x40, true); // single command
		
		
		for (uint8_t i = 0; i < 132; i++)
		{
			i2c_master_write_byte(cmd2, 0xFF >> (i % 8) , true); 
		}
		i2c_master_stop(cmd2);
		i2c_master_cmd_begin(I2C_NUM_0, cmd2, 10 / portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd2);
	}  */

 
 sh1106_t display;
    display.addr = OLED_ADDR;
    display.port = I2C_NUM_0;

 for (uint8_t y = 0; y < 128; y++) {
        for (uint8_t x = 0; x < 128; x++) {
            sh1106_set(&display, x, y, 0xFF);
        }
    }
 
sh1106_update(&display);

	
   
	 while (true){



	 }
}


