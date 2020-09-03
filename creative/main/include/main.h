#ifndef MAIN_H
# define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <string.h>
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

#include "sound_amplifier.h"
#include "oled.h"
#include "dht.h"
#include "accelerometer.h"

#endif