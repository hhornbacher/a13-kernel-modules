#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>

#include <mach/sys_config.h>

#include "w1.h"
#include "w1_int.h"

#include "w1-sun5i.h"

static unsigned gpio_handler;
static script_gpio_set_t info;

static int __init w1_sun5i_probe_driver(struct platform_device *pdev) {
    int err = 0;
    int w1_used = 0;
    struct w1_bus_master *master;

/*
    if (!pdata) {
        printk(KERN_INFO "%s: Invalid platform_data!\n", __FUNCTION__);
        return -ENXIO;
    }
 
 
    master = kzalloc(sizeof (struct w1_bus_master), GFP_KERNEL);
    if (!master) {
        printk(KERN_INFO "%s: Not enough memory!\n", __FUNCTION__);
        kfree(master);
        return -ENOMEM;
    }
    
    err = script_parser_fetch("w1_para", "w1_used", &w1_used, sizeof (w1_used) / sizeof (int));
    if (!w1_used || err) {
        printk(KERN_INFO "%s: w1-bus is not used in config\n", __FUNCTION__);
        kfree(master);
        return -1;
    }

    err = script_parser_fetch("w1_para", "w1_pin", (int *) &info, sizeof (script_gpio_set_t));
    gpio_handler = gpio_request_ex("w1_para", "w1_pin");
    if (!gpio_handler || err) {
        printk(KERN_INFO "%s: can not get \"w1_para\" \"w1_pin\" gpio handler, already used by others?", __FUNCTION__);
        kfree(master);
        gpio_release(gpio_handler, 0);
        return -1;
    }
    
    printk(KERN_INFO "%s: w1-bus GPIO pin: port:%d, portnum:%d\n", __FUNCTION__, info.port, info.port_num);
*/

    return 0;
}

static int __exit w1_sun5i_remove_driver(struct platform_device *pdev) {
/*
    gpio_release(gpio_handler, 0);
*/
    return 0;
}

static struct platform_device sun5i_w1_master_davice = {
	.name = DRIVER_NAME,
};

static struct platform_driver w1_sun5i_driver = {
    .driver =
    {
        .name = DRIVER_NAME,
    },
    .remove = w1_sun5i_remove_driver
};

static struct platform_device *w1_pdevs[] __initdata = {
	&sun5i_w1_master_davice,
};

static int __init w1_sun5i_init_driver(void) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    platform_add_devices(w1_pdevs, ARRAY_SIZE(w1_pdevs));
    return platform_driver_probe(&w1_sun5i_driver, w1_sun5i_probe_driver);
    return 0;
}

static void __exit w1_sun5i_exit_driver(void) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    platform_driver_unregister(&w1_sun5i_driver);
}


MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(w1_sun5i_init_driver);
module_exit(w1_sun5i_exit_driver);
