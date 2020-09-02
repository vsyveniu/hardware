#ifndef ACCELEROMETER_H
# define ACCELEROMETER_H

#include "driver/spi_common.h"
#include "driver/spi_master.h"

void accel_init(spi_device_handle_t spi);

#endif
