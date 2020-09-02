#ifndef DEFINES_H
# define DEFINES_H

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

#define OLED_NORMAL 0
#define OLED_REVERSE 1

#define BUTT_1 	GPIO_NUM_39
#define BUTT_2 	GPIO_NUM_18

#endif