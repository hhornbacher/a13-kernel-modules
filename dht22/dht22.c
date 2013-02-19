/*
 *  dht22.c
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <mach/sys_config.h>

#include "dht22.h"

static unsigned gpio_handler;
static script_gpio_set_t info;

#define PIN_DIR(dir) gpio_set_one_pin_io_status(gpio_handler, dir, NULL);
#define PIN_SET() gpio_write_one_pin_value(gpio_handler, val, NULL);
#define PIN_GET(val) gpio_read_one_pin_value(gpio_handler, NULL);

static void dht22_exit_driver(void) {
    gpio_release(gpio_handler, 0);
    printk(KERN_INFO "%s: Unloaded DHT22 driver\n", __func__);
}

static int dht22_init_driver(void) {
    int err = 0;
    int used = 0;
    
    err = script_parser_fetch("dht22_para", "dht22_used", &used, sizeof (used));
    if(err || used==0) {
        printk(KERN_INFO "%s: Cannot setup DHT22 driver, maybe not configured in script.bin?", __FUNCTION__);
    }

    // Get GPIO Pin config from script.bin
    err = script_parser_fetch("dht22_para", "dht22_pin", (int *) &info, sizeof (script_gpio_set_t));
    gpio_handler = gpio_request_ex("dht22_para", "dht22_pin");
    if (!gpio_handler || err) {
        printk(KERN_INFO "%s: can not get \"dht22_para\" \"dht22_pin\" gpio handler, already used by others?", __FUNCTION__);
        goto exit;
    }

    return 0;
exit:
    dht22_exit_driver();
    return -1;
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(dht22_init_driver);
module_exit(dht22_exit_driver);

