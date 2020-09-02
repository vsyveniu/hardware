#ifndef IGNITE_H
# define IGNITE_H

#include "defines.h"
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
#include "driver/i2c.h"

int ignite_parts();

#endif