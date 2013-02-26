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
#include <linux/proc_fs.h>
#include <asm/uaccess.h> 

#include <linux/timer.h>
#include <linux/delay.h>

#include <asm/delay.h>

#include <mach/sys_config.h>

#include "dht22-sun5i.h"

static struct proc_dir_entry *dht22_sun5i_proc;
static unsigned *dht22_sun5i_gpio_handler;
static int dht22_sun5i_gpio_direction;
static struct dht22_sun5i_sensor_data dht22_sun5i_data;

static int dht22_sun5i_proc_read(char* buffer, char** start, off_t off, int count, int* peof, void* data) {
    if(dht22_sun5i_sensor_read()==0)
        return sprintf(buffer, "rh_i=%d\nrh_d=%d\ntp_i=%d\ntp_d=%d\nchecksum=%d\n", dht22_sun5i_data.rh_i, dht22_sun5i_data.rh_d, dht22_sun5i_data.tp_i, dht22_sun5i_data.tp_d, dht22_sun5i_data.checksum);
    else
        return sprintf(buffer, "N/A");
}

/* Init driver */
static int __init dht22_sun5i_init_driver() {
    int err = 0;
    int dht22_used = 0;
    script_gpio_set_t info;

    dht22_sun5i_proc = proc_mkdir("dht22", NULL);
    create_proc_read_entry("raw_data", 0444, dht22_sun5i_proc, dht22_sun5i_proc_read, NULL);

    err = script_parser_fetch("dht22_para", "dht22_used", &dht22_used, sizeof (dht22_used) / sizeof (int));
    if (!dht22_used || err) {
        printk(KERN_INFO "%s: dht22-bus is not used in config\n", __FUNCTION__);
        return -EINVAL;
    }

    err = script_parser_fetch("dht22_para", "dht22_pin", (int *) &info, sizeof (script_gpio_set_t));
    if (err) {
        printk(KERN_INFO "%s: can not get \"dht22_para\" \"dht22_pin\" gpio handler, already used by others?", __FUNCTION__);
        return -EBUSY;
    }
    dht22_sun5i_gpio_handler = gpio_request_ex("dht22_para", "dht22_pin");
    dht22_sun5i_gpio_direction = PIN_DIR_OUT;
    PIN_DIR(PIN_DIR_OUT);

    return 0;
}

/*Cleanup*/
static int dht22_sun5i_exit_driver() {
    remove_proc_entry("raw_data", dht22_sun5i_proc);
    remove_proc_entry("dht22", NULL);
    gpio_release(dht22_sun5i_gpio_handler, 0);
    return 0;
}

static int dht22_sun5i_sensor_read() {
    int state = 0;
    struct timeval end, start;
    long diff;
    int bits[40], i;

    // Init host
    dht22_sun5i_write_bit(1);
    mdelay(200);
    dht22_sun5i_write_bit(0);
    udelay(520);
    dht22_sun5i_write_bit(1);
    udelay(25);

    // Init sensor
    // Read data (low 80 us)
    do_gettimeofday(&start);
    while (state == 0) {
        state = dht22_sun5i_read_bit();
    }
    do_gettimeofday(&end);
    diff = end.tv_usec - start.tv_usec;
    do_gettimeofday(&start);
    if (diff < 59) {
        printk(KERN_INFO "1, diff=%d", diff);
        return -1;
    }
    // Read data (high 80 us)
    while (state == 1) {
        state = dht22_sun5i_read_bit();
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
            state = dht22_sun5i_read_bit();
        }
        do_gettimeofday(&end);
        diff = end.tv_usec - start.tv_usec;
        do_gettimeofday(&start);
        if (diff < 40) {
            printk(KERN_INFO "#%d: 1, diff=%d", i, diff);
            return -1;
        }
        while (state == 1) {
            state = dht22_sun5i_read_bit();
        }
        do_gettimeofday(&end);
        diff = end.tv_usec - start.tv_usec;
        do_gettimeofday(&start);
        if (diff >= 23 && diff <= 32)
            bits[i] = 0;
        else if (diff >= 63 && diff <= 80)
            bits[i] = 1;
        else {
            printk(KERN_INFO "#%d: 2, diff=%d", i, diff);
            return -1;
        }
    }
    dht22_sun5i_data.rh_i = (bits[7]) | (bits[6] << 1) | (bits[5] << 2) | (bits[4] << 3) | (bits[3] << 4) | (bits[2] << 5) | (bits[1] << 6) | (bits[0] << 7);
    dht22_sun5i_data.rh_d = (bits[15]) | (bits[14] << 1) | (bits[13] << 2) | (bits[12] << 3) | (bits[11] << 4) | (bits[10] << 5) | (bits[9] << 6) | (bits[8] << 7);
    dht22_sun5i_data.tp_i = (bits[23]) | (bits[22] << 1) | (bits[21] << 2) | (bits[20] << 3) | (bits[19] << 4) | (bits[18] << 5) | (bits[17] << 6) | (bits[16] << 7);
    dht22_sun5i_data.tp_d = (bits[31]) | (bits[30] << 1) | (bits[29] << 2) | (bits[28] << 3) | (bits[27] << 4) | (bits[26] << 5) | (bits[25] << 6) | (bits[24] << 7);
    dht22_sun5i_data.checksum = (bits[39]) | (bits[38] << 1) | (bits[37] << 2) | (bits[36] << 3) | (bits[35] << 4) | (bits[34] << 5) | (bits[33] << 6) | (bits[32] << 7);
    dht22_sun5i_data.valid = 0;
    if ((dht22_sun5i_data.checksum == ((dht22_sun5i_data.rh_i + dht22_sun5i_data.rh_d + dht22_sun5i_data.tp_i + dht22_sun5i_data.tp_d) & 0xFF))) {
        dht22_sun5i_data.valid = 1;
    }
    return 0;
}

static void dht22_sun5i_write_bit(u8 bit) {
    if (dht22_sun5i_gpio_direction != PIN_DIR_OUT) {
        dht22_sun5i_gpio_direction = PIN_DIR_OUT;
        PIN_DIR(PIN_DIR_OUT);
    }
    PIN_SET(bit);
}

static u8 dht22_sun5i_read_bit() {
    if (dht22_sun5i_gpio_direction != PIN_DIR_IN) {
        dht22_sun5i_gpio_direction = PIN_DIR_IN;
        PIN_DIR(PIN_DIR_IN);
    }
    PIN_DIR(0);
    return PIN_GET();
}


MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(dht22_sun5i_init_driver);
module_exit(dht22_sun5i_exit_driver);
