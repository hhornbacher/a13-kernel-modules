/*
 *  gpio-test.c
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <mach/sys_config.h>

#include "gpio-test.h"

static unsigned gpio_handler;
static script_gpio_set_t info;

static void __exit gpio_test_exit_driver(void) {
    gpio_release(gpio_handler, 0);
    printk(KERN_INFO "%s: Unloaded GPIO test driver\n", __func__);
}

static int __init gpio_test_init_driver(void) {
    int err = 0;

    // Get GPIO Pin config from script.bin
    err = script_parser_fetch("gpt_para", "gpt_pin", (int *) &info, sizeof (script_gpio_set_t));
    gpio_handler = gpio_request_ex("gpt_para", "gpt_pin");
    if (!gpio_handler || err) {
        printk(KERN_INFO "%s can not get \"gpt_para\" \"gpt_pin\" gpio handler, already used by others?", __FUNCTION__);
        goto exit;
    }

    // Set GPIO to output
    gpio_set_one_pin_io_status(gpio_handler, 1, info.gpio_name);

    gpio_write_one_pin_value(gpio_handler, 1, info.gpio_name);

    printk(KERN_INFO "%s: Loaded GPIO test driver\n", __func__);
    return 0;

exit:
    gpio_test_exit_driver();
    return -1;
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(gpio_test_init_driver);
module_exit(gpio_test_exit_driver);
