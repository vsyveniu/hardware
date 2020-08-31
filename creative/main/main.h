#ifndef MAIN_H
# define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <esp32/rom/ets_sys.h>
#include "esp_err.h"
#include <string.h>
#include <inttypes.h>
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "math.h"
#include <driver/dac.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "font6x8.h"
#include <stdlib.h>


typedef struct {
    uint8_t page_orientation;
} oled_settings_t;

#endif