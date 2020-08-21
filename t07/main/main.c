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

void get_char(uint8_t *arr, int sym){
	uint8_t w = 0;
	int start = (sym - 32) * 6 + 0;
	while(w < 6){
		arr[w] = font6x8[start];
		printf("%x\n", arr[w]);
		start++;
		w++;
	}

	
	//int charmap[6] =
	//	y * w + x
}
/* 
void create_load(uint8_t *arr, char *str, int len){
	int i = 0;
	int j = 0;
	uint8_t w = 0;
	int start = 0;
	 while(i < len){
		 start = (str[i] - 32) * 6 + 0;
		 //printf("%d  start\n", start);
		 //printf("%d  j\n", j);
		for (w = 0; w < 6; w++){
			 
			arr[j] = 0x00;
			arr[j] = font6x8[start];
			start++;
			//printf("%x  arr j \n", arr[j]);
			j++;
		}
		for(w = 6; w < 8; w++){
			arr[j] = 0b00000000;
			//printf("%x  arr j \n", arr[j]);
			j++;
			
		}
		i++;


		//get_char(arr, str[i]);
		//get_char(&arr[i], str[i]);
		//i++;
	}
}
 */
void create_load(uint8_t *arr, char *str, int len){
	int start = 0;
	int j = 0;
	int w = 8;
	int i = 0;
	for(int k = 0; k < len; k++){
		start = (str[k] - 32) * 6 + 0;
		printf("%d start", start);
		w = 8;
		while (i < 128){
			if(w == 0){
					break;
				}
				if(w <= 2){
					arr[i] = 0x00;
				}
				else
					arr[i] = font6x8[start];
				start++;
				w--;
				printf("%x\n", arr[i]);
				printf("%d - w\n", w);
				i++;
		}		
		
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


void app_main(void){

	//get_char();

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


	
 
	uint8_t buff[8][128];

	 for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  buff[y][x] = 0;

        }
    }

 for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  buff[y][x] = 0b00000000;
			  write_page(&buff[y][x], y);
            //sh1106_set(&display, x, y, 0xFF);
        }
    }

	uint8_t arr2[8][128];

/* int fuck = 0;
	while(fuck < 16){
		printf("%x\n", arr2[fuck]);
		fuck++;
	} */

	char *test = "Huli meni rogue ";
	char *test1 = "legacy";
	char *test2 = "yaksho ne robe";
	printf("%d  LENt\n", strlen(test));

 for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  arr2[y][x] = 0b00000000;
			  write_page(&buff[y][x], y);
            //sh1106_set(&display, x, y, 0xFF);
        }
    }

	 //for (uint8_t y = 0; y < 8; y++) {

		create_load(arr2[0], test, strlen(test));

		create_load(arr2[1], test1, strlen(test1));

		create_load(arr2[2], test2, strlen(test2));

    //}

		

	
	

	
	 int fuck = 0;


	 for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			// printf("%x\n", arr2[y][x]);
            //sh1106_set(&display, x, y, 0xFF);
        }
    }


	//printf("%dfuck\n", fuck);

	//get_char(arr, 'F');

/* 
	for (size_t i = 0; i < 8; i++)
	{
		printf("%x\n", arr[i]);
	}
	 */

/* for (size_t i = 0; i < 8; i++)
{
	write_page(&arr[i], 2);
	
}

	bzero(arr, 8);
	get_char(arr, 'U');

for (size_t i = 0; i < 8; i++)
{
	write_page(&arr[i], 2);
	
}

	bzero(arr, 8);
	get_char(arr, 'C');

for (size_t i = 0; i < 8; i++)
{
	write_page(&arr[i], 2);
	
} */

//	bzero(arr, 8);
//	**get_char(arr, 'K');


 //for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 128; x++) {
			  write_page(&arr2[0][x], 1);
            //sh1106_set(&display, x, y, 0xFF);
        }

		 for (uint8_t x = 0; x < 128; x++) {
			  write_page(&arr2[1][x], 3);
            //sh1106_set(&display, x, y, 0xFF);
        }

		for (uint8_t x = 0; x < 128; x++) {
			  write_page(&arr2[2][x], 5);
            //sh1106_set(&display, x, y, 0xFF);
        }
   // }


vTaskDelay(200 / portTICK_PERIOD_MS);


 //for (int y = 0; y < 8; y++) {
	 /*  for (int x = 0; x < 128; x++) {
			  buff[y][x] = 0b1110000;
			  printf("%uy\n", y);
			  printf("%u\n", x);
			  //printf("%x\n", buff[y][x]);
			  vTaskDelay(5 / portTICK_PERIOD_MS);
			  write_page(&buff[y][x], 5);
            //sh1106_set(&display, x, y, 0xFF);
        }  */
   /*      for (int x = 50; x < 80; x++) {
			  buff[y][x] = 0b10000010;
			  printf("%dy\n", y);
			  printf("%u\n", x);
			  //printf("%x\n", buff[y][x]);
			  vTaskDelay(5 / portTICK_PERIOD_MS);
			  write_page(&buff[y][x], 5);
            //sh1106_set(&display, x, y, 0xFF);
        }
		for (int x = 80; x < 128; x++) {
			  buff[y][x] = 0b00000000;
			  printf("%dy\n", y);
			  printf("%u\n", x);
			 // printf("%x\n", buff[y][x]);
			  vTaskDelay(5 / portTICK_PERIOD_MS);
				write_page(&buff[y][x], 5);
            //sh1106_set(&display, x, y, 0xFF);
        } */
 //g   }
 
//sh1106_update(&display);

	
   
	 while (true){



	 }
}


