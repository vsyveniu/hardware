#include "main.h"
#define EN_OLED GPIO_NUM_32
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define OLED_ADDR 0x3c




void app_main(void){

	esp_err_t err;
	i2c_cmd_handle_t cmd;

	gpio_set_direction(EN_OLED, GPIO_MODE_OUTPUT);
	gpio_set_level(EN_OLED, 1);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	i2c_config_t i2c_conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_SDA,
		.scl_io_num = I2C_SCL,
		.sda_pullup_en = true,
		.scl_pullup_en = true,
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

	err = i2c_master_write_byte(cmd, 0x00, true);
	if(err != ESP_OK){
		printf("%s", "fuck");
	}

	i2c_master_write_byte(cmd, 0xAD, true);
	i2c_master_write_byte(cmd, 0x0B, true);

	i2c_master_write_byte(cmd, 0xAF, true);


/* 
	i2c_master_write_byte(cmd, 0x00, true); // reset column low bits
	i2c_master_write_byte(cmd, 0x10, true); // reset column high bits */
	i2c_master_write_byte(cmd, 0xB0, true); // reset page
    i2c_master_write_byte(cmd, 0x40, true); // set start line
     i2c_master_write_byte(cmd, 0xD3, true);
    i2c_master_write_byte(cmd, 0x00, true);	
	i2c_master_write_byte(cmd, 0x81, true); 
	//i2c_master_write_byte(cmd, 0x19, true);
	//i2c_master_write_byte(cmd, 0x0B, true);

	i2c_master_stop(cmd);

	err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	if (err == ESP_OK) {
		ESP_LOGI("tag", "OLED configured successfully");
	} else {
		ESP_LOGE("tag", "OLED configuration failed. code: 0x%.2X", err);
	}
	i2c_cmd_link_delete(cmd);



for (uint8_t i = 0; i < 8; i++) {
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
		i2c_master_write_byte(cmd, 0x80, true);
		i2c_master_write_byte(cmd, 0xB0 | i, true);
		i2c_master_write_byte(cmd, 0x40, true);
		for (uint8_t j = 0; j < 132; j++) {
			i2c_master_write_byte(cmd, 0xFF >> (j % 8), true);
		}
		i2c_master_stop(cmd);
		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);
	}

   
	 while (true){



	 }
}


