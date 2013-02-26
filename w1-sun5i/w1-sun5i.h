/*
 * w1-sun5i - W1 master driver for sun5i platform
 *
 * Copyright (C) 2013 Harry Hornbacher <h.hornbacher@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#define DRIVER_AUTHOR   "Harry Hornbacher <h.hornbacher@gmail.com>"
#define DRIVER_DESC     "W1 driver for sun5i platform"
#define DRIVER_NAME     "w1-sun5i"
#define DRV_VERSION     "0.0.1"


#define PIN_DIR(handler,dir) gpio_set_one_pin_io_status(handler, dir, NULL);
#define PIN_SET(handler,val) gpio_write_one_pin_value(handler, val, NULL);
#define PIN_GET(handler) gpio_read_one_pin_value(handler, NULL);

struct w1_sun5i_platform_data {
    unsigned gpio_handler;
    script_gpio_set_t info;
};

static int w1_sun5i_remove_driver(struct platform_device *pdev);
static int w1_sun5i_probe_driver(struct platform_device *pdev);


static void w1_sun5i_write_bit(void *data, u8 bit);
static u8 w1_sun5i_read_bit(void *data);
