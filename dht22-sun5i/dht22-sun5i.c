/*
 * dht22-sun5i - DHT22 driver for sun5i platform
 *
 * Copyright (C) 2013 Harry Hornbacher <h.hornbacher@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>

#include <linux/timer.h>

#include <asm/delay.h>

#include <mach/sys_config.h>

#include "dht22-sun5i.h"

static struct platform_device *dht22_sun5i_device = NULL;
static struct platform_driver dht22_sun5i_driver = {
    .driver =
    {
        .name = DRIVER_NAME,
    },
    .remove = dht22_sun5i_remove_driver
};

/* Init driver */
static int dht22_sun5i_probe_driver(struct platform_device *pdev) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    int err = 0;
    int dht22_used = 0;
    struct dht22_sun5i_platform_data *pdata = pdev->dev.platform_data;

    if (!pdata) {
        printk(KERN_INFO "%s: Invalid platform_data!\n", __FUNCTION__);
        return -ENXIO;
    }

    err = script_parser_fetch("dht22_para", "dht22_used", &dht22_used, sizeof (dht22_used) / sizeof (int));
    if (!dht22_used || err) {
        printk(KERN_INFO "%s: dht22-bus is not used in config\n", __FUNCTION__);
        return -EINVAL;
    }

    err = script_parser_fetch("dht22_para", "dht22_pin", (int *) &pdata->info, sizeof (script_gpio_set_t));
    if (err) {
        printk(KERN_INFO "%s: can not get \"dht22_para\" \"dht22_pin\" gpio handler, already used by others?", __FUNCTION__);
        return -EBUSY;
    }
    pdata->gpio_handler = gpio_request_ex("dht22_para", "dht22_pin");
    pdata->direction = PIN_DIR_OUT;
    PIN_DIR(pdata->gpio_handler, PIN_DIR_OUT);

    return dht22_sun5i_sensor_read(pdata);

    return 0;
}

/*Cleanup*/
static int dht22_sun5i_remove_driver(struct platform_device *pdev) {
    struct dht22_sun5i_platform_data *pdata = pdev->dev.platform_data;
    gpio_release(pdata->gpio_handler, 0);
    kfree(dht22_sun5i_device->dev.platform_data);
    platform_device_unregister(dht22_sun5i_device);
    return 0;
}

static int dht22_sun5i_sensor_read(struct dht22_sun5i_platform_data *pdata) {
    int state = 0;
    struct timeval end, start;
    long diff;
    int bits[40], i;

    // Init host
    dht22_sun5i_write_bit(pdata, 0);
    udelay(500);
    dht22_sun5i_write_bit(pdata, 1);
    udelay(25);

    // Init sensor
    // Read data (low 80 us)
    do_gettimeofday(&start);
    while (state == 0) {
        state = dht22_sun5i_read_bit(pdata);
    }
    do_gettimeofday(&end);
    diff = end.tv_usec - start.tv_usec;
    do_gettimeofday(&start);
    if (diff < 65) {
        printk(KERN_INFO "1, diff=%d", diff);
        return -1;
    }
    // Read data (high 80 us)
    while (state == 1) {
        state = dht22_sun5i_read_bit(pdata);
    }
    do_gettimeofday(&end);
    diff = end.tv_usec - start.tv_usec;
    do_gettimeofday(&start);
    if (diff < 65) {
        printk(KERN_INFO "2, diff=%d", diff);
        return -1;
    }

    for (i = 0; i < 40; i++) {
        while (state == 0) {
            state = dht22_sun5i_read_bit(pdata);
        }
        do_gettimeofday(&end);
        diff = end.tv_usec - start.tv_usec;
        do_gettimeofday(&start);
        if (diff < 40) {
            printk(KERN_INFO "#%d: 1, diff=%d", i, diff);
            return -1;
        }
        while (state == 1) {
            state = dht22_sun5i_read_bit(pdata);
        }
        do_gettimeofday(&end);
        diff = end.tv_usec - start.tv_usec;
        do_gettimeofday(&start);
        if (diff >= 24 && diff <= 28)
            bits[i] = 0;
        else if (diff >= 66 && diff <= 80)
            bits[i] = 1;
        else {
            printk(KERN_INFO "#%d: 2, diff=%d", i, diff);
            return -1;
        }
    }
    struct dht22_sun5i_sensor_data data;
    data.rh_i = (bits[7]) | (bits[6] << 1) | (bits[5] << 2) | (bits[4] << 3) | (bits[3] << 4) | (bits[2] << 5) | (bits[1] << 6) | (bits[0] << 7);
    data.rh_d = (bits[15]) | (bits[14] << 1) | (bits[13] << 2) | (bits[12] << 3) | (bits[11] << 4) | (bits[10] << 5) | (bits[9] << 6) | (bits[8] << 7);
    data.tp_i = (bits[23]) | (bits[22] << 1) | (bits[21] << 2) | (bits[20] << 3) | (bits[19] << 4) | (bits[18] << 5) | (bits[17] << 6) | (bits[16] << 7);
    data.tp_d = (bits[31]) | (bits[30] << 1) | (bits[29] << 2) | (bits[28] << 3) | (bits[27] << 4) | (bits[26] << 5) | (bits[25] << 6) | (bits[24] << 7);
    data.checksum = (bits[39]) | (bits[38] << 1) | (bits[37] << 2) | (bits[36] << 3) | (bits[35] << 4) | (bits[34] << 5) | (bits[33] << 6) | (bits[32] << 7);
    data.valid = 0;
    if ((data.checksum == ((data.rh_i + data.rh_d + data.tp_i + data.tp_d) & 0xFF))) {
        data.valid = 1;
    }
    
    printk(KERN_INFO "data: rh_i=%d, rh_d=%d, tp_i=%d, tp_d=%d, checksum=0x%x, valid=%d", data.rh_i, data.rh_d, data.tp_i, data.tp_d, data.checksum, data.valid);

    
    return 0;
}

static void dht22_sun5i_write_bit(struct dht22_sun5i_platform_data *pdata, u8 bit) {
    if (pdata->direction != PIN_DIR_OUT) {
        pdata->direction = PIN_DIR_OUT;
        PIN_DIR(pdata->gpio_handler, PIN_DIR_OUT);
    }
    PIN_SET(pdata->gpio_handler, bit);
}

static u8 dht22_sun5i_read_bit(struct dht22_sun5i_platform_data *pdata) {
    if (pdata->direction != PIN_DIR_IN) {
        pdata->direction = PIN_DIR_IN;
        PIN_DIR(pdata->gpio_handler, PIN_DIR_IN);
    }
    PIN_DIR(pdata->gpio_handler, 0);
    return PIN_GET(pdata->gpio_handler);
}

static int __init dht22_sun5i_init_driver(void) {
    dht22_sun5i_device = platform_device_register_simple(DRIVER_NAME, 0, NULL, 0);
    dht22_sun5i_device->dev.platform_data = kzalloc(sizeof (struct dht22_sun5i_platform_data), GFP_KERNEL);
    return platform_driver_probe(&dht22_sun5i_driver, dht22_sun5i_probe_driver);
}

static void __exit dht22_sun5i_exit_driver(void) {
    platform_driver_unregister(&dht22_sun5i_driver);
}


MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(dht22_sun5i_init_driver);
module_exit(dht22_sun5i_exit_driver);
