/*
 * w1-gpio - GPIO w1 bus master driver
 *
 * Copyright (C) 2007 Ville Syrjala <syrjala@sci.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <mach/sys_config.h>

#include "w1.h"
#include "w1_int.h"
#include "sunxi-w1.h"


static unsigned gpio_handler;
static script_gpio_set_t info;

static void w1_gpio_write_bit_dir(void *data, u8 bit) {
    int ret;

    ret = gpio_set_one_pin_io_status(gpio_handler, 1, info.gpio_name);
    if (!ret)
        gpio_set_one_pin_io_status(gpio_handler, bit, info.gpio_name);
}

static void w1_gpio_write_bit_val(void *data, u8 bit) {
    gpio_write_one_pin_value(gpio_handler, bit, info.gpio_name);
}

static u8 w1_gpio_read_bit(void *data) {
    return gpio_read_one_pin_value(gpio_handler, info.gpio_name);
}

static int __init w1_gpio_probe(struct platform_device *pdev) {
    int err = 0;
    int i;
    int w1_used = 0;
    struct w1_bus_master *master;
    struct w1_gpio_platform_data *pdata = pdev->dev.platform_data;

    if (!pdata)
        return -ENXIO;

    master = kzalloc(sizeof (struct w1_bus_master), GFP_KERNEL);
    if (!master)
        return -ENOMEM;

    err = script_parser_fetch("w1_para", "w1_used", &w1_used, sizeof (w1_used) / sizeof (int));
    if (!w1_used || err) {
        printk(KERN_INFO "%s w1-bus is not used in config\n", __FUNCTION__);
        err = -1;
        goto exit;
    }

    err = script_parser_fetch("w1_para", "w1_pin", (int *) &info, sizeof (script_gpio_set_t));
    gpio_handler = gpio_request_ex("w1_para", "w1_pin");
    if (!gpio_handler || err) {
        printk(KERN_INFO "%s can not get \"w1_para\" \"w1_pin\" gpio handler, already used by others?", __FUNCTION__);
        goto exit;
    }

    printk(KERN_INFO "w1-bus on w1_pins: port:%d, portnum:%d\n", info.port, info.port_num);

    master->data = pdata;
    master->read_bit = w1_gpio_read_bit;

    if (pdata->is_open_drain) {
        gpio_set_one_pin_io_status(gpio_handler, 1, info.gpio_name);
        master->write_bit = w1_gpio_write_bit_val;
    } else {
        gpio_set_one_pin_io_status(gpio_handler, 0, info.gpio_name);
        master->write_bit = w1_gpio_write_bit_dir;
    }

    err = w1_add_master_device(master);
    if (err)
        goto free_gpio;

    if (pdata->enable_external_pullup)
        pdata->enable_external_pullup(1);

    platform_set_drvdata(pdev, master);

    return 0;

free_gpio:
    gpio_release(gpio_handler, 0);
free_master:
    kfree(master);

    return err;
    return 0;


exit:
    return err;
}

static int __exit w1_gpio_remove(struct platform_device *pdev) {
    struct w1_bus_master *master = platform_get_drvdata(pdev);
    struct w1_gpio_platform_data *pdata = pdev->dev.platform_data;

    if (pdata->enable_external_pullup)
        pdata->enable_external_pullup(0);

    w1_remove_master_device(master);
    gpio_release(gpio_handler, 0);
    kfree(master);


    printk(KERN_INFO "Removed: w1-bus on w1_pin [%d,%d]\n", info.port, info.port_num);
    return 0;
}

static struct platform_driver w1_gpio_driver = {
    .driver =
    {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
    .remove = __exit_p(w1_gpio_remove),
};

static int __init w1_gpio_init(void) {
    return platform_driver_probe(&w1_gpio_driver, w1_gpio_probe);
}

static void __exit w1_gpio_exit(void) {
    platform_driver_unregister(&w1_gpio_driver);
}

module_init(w1_gpio_init);
module_exit(w1_gpio_exit);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);