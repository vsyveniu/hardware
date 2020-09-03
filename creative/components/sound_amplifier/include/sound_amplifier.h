#ifndef SOUND_AMPLIFIER_H
# define SOUND_AMPLIFIER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <esp32/rom/ets_sys.h>
#include <driver/dac.h>

void beep();

#endif
