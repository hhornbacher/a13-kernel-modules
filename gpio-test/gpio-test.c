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

static unsigned gpio_handler1,gpio_handler2;
static script_gpio_set_t info1,info2;

static int 

static void __exit gpio_test_exit_driver(void) {
    gpio_release(gpio_handler1, 0);
    gpio_release(gpio_handler2, 0);
    printk(KERN_INFO "%s: Unloaded GPIO test driver\n", __func__);
}

static int __init gpio_test_init_driver(void) {
    int err = 0;

    // Get GPIO Pin config from script.bin
    err = script_parser_fetch("gpt_para", "gpt_pin_1", (int *) &info1, sizeof (script_gpio_set_t));
    gpio_handler1 = gpio_request_ex("gpt_para", "gpt_pin_1");
    if (!gpio_handler1 || err) {
        printk(KERN_INFO "%s: can not get \"gpt_para\" \"gpt_pin_1\" gpio handler, already used by others?", __FUNCTION__);
        goto exit;
    }
    
    // Get GPIO Pin config from script.bin
    err = script_parser_fetch("gpt_para", "gpt_pin_2", (int *) &info2, sizeof (script_gpio_set_t));
    gpio_handler2 = gpio_request_ex("gpt_para", "gpt_pin_2");
    if (!gpio_handler2 || err) {
        printk(KERN_INFO "%s: can not get \"gpt_para\" \"gpt_pin_2\" gpio handler, already used by others?", __FUNCTION__);
        goto exit;
    }
    
    

    // Set GPIO to output
    err = gpio_set_one_pin_io_status(gpio_handler1, 1, NULL);
    if (err!=0) {
        printk(KERN_INFO "%s: Failed to set GPIO io status to output.", __FUNCTION__);
        goto exit;
    }

    err = gpio_write_one_pin_value(gpio_handler1, 1, NULL);
    if (err!=0) {
        printk(KERN_INFO "%s: Failed to write to GPIO pin.", __FUNCTION__);
        goto exit;
    }
    int data = gpio_read_one_pin_value(gpio_handler1, NULL);
    printk(KERN_INFO "%s: GPIO pin1 value: %d", __FUNCTION__, data );
    int data = gpio_read_one_pin_value(gpio_handler2, NULL);
    printk(KERN_INFO "%s: GPIO pin2 value: %d", __FUNCTION__, data );

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
