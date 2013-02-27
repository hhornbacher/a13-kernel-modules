/* 
 * File:   gpio_makros.h
 * Author: hhornbacher
 *
 * Created on 26. Februar 2013, 23:47
 */

#ifndef GPIO_MAKROS_H
#define	GPIO_MAKROS_H

// GPIO makros
#define PIN_DIR(handler,dir) gpio_set_one_pin_io_status(handler, dir, NULL);
#define PIN_SET(handler,val) gpio_write_one_pin_value(handler, val, NULL);
#define PIN_GET(handler) gpio_read_one_pin_value(handler, NULL);
#define PIN_DIR_OUT                 1
#define PIN_DIR_IN                  0

#endif	/* GPIO_MAKROS_H */

