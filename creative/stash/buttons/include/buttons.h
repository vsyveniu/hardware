#ifndef BUTTONS_H
# define BUTTONS_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <esp32/rom/ets_sys.h>
#include "esp_err.h"
#include <inttypes.h>


void butt1_pushed();
void butt2_pushed();

#endif