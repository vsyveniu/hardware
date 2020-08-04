#ifndef MAIN_H
# define MAIN_H

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "math.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include <driver/adc.h>
#include "driver/ledc.h"

typedef struct s_person
{
    char firstName[20];
    char lastName[20];
    int  age;
}               t_person;

int moduleFunction(int a, int b);


#endif