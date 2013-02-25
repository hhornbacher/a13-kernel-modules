/* 
 * File:   w1-sun5i.h
 * Author: hhornbacher
 *
 * Created on 23. Februar 2013, 18:36
 */



#define DRIVER_AUTHOR   "Harry Hornbacher <h.hornbacher@gmail.com>"
#define DRIVER_DESC     "W1 driver for sun5i devices"
#define DRIVER_NAME     "w1-sun5i"
#define DRV_VERSION     "0.0.1"

struct w1_sun5i_platform_data {
    unsigned gpio_handler;
    script_gpio_set_t info;
};

static int w1_sun5i_remove_driver(struct platform_device *pdev);
static int w1_sun5i_probe_driver(struct platform_device *pdev);


static void w1_sun5i_write_bit(void *data, u8 bit);
static u8 w1_sun5i_read_bit(void *data);
