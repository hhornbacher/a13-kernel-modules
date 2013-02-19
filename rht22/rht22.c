/*
 *  rht22.c
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "rht22.h"

static void rht22_exit_driver(void) {
    
}

static int rht22_init_driver(void) {
    return 0;
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(rht22_init_driver);
module_exit(rht22_exit_driver);

