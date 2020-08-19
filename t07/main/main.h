#ifndef MAIN_H
# define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <esp32/rom/ets_sys.h>
#include "esp_err.h"
#include <string.h>
#include <driver/dac.h>
#include <inttypes.h>
#include "driver/i2c.h"
#include "font6x8.h"

typedef struct {
    uint8_t addr;           // I2C address
    i2c_port_t port;        // I2C interface port
    uint16_t changes;       // page change bit to optimize writes
    uint8_t pages[16][128]; // page buffer
} sh1106_t;



#endif