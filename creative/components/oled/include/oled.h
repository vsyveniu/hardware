#ifndef OLED_H
# define OLED_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <string.h>
#include <inttypes.h>
#include "driver/i2c.h"
#include "defines.h"

void display_str(char *str, int page, int appear_speed, int font_weight);
void create_load(uint8_t *arr, char *str, int len, int font_weight);
void write_page(uint8_t *data, uint8_t page);
int init_oled();
void clear_oled();

#endif
