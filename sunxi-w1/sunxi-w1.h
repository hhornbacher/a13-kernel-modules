/* 
 * File:   sunxi-w1.h
 * Author: hhornbacher
 *
 * Created on 17. Februar 2013, 10:17
 */

#define DRIVER_AUTHOR   "Harry Hornbacher <h.hornbacher@gmail.com>"
#define DRIVER_DESC     "W1 driver for sunxi devices"
#define DRIVER_NAME     "sunxi-w1"
#define DRV_VERSION     "0.0.1"


struct w1_gpio_platform_data {
	unsigned int pin;
	unsigned int is_open_drain:1;
	void (*enable_external_pullup)(int enable);
};