/* 
 * File:   lcd1602.h
 * Author: hhornbacher
 *
 * Created on 23. Februar 2013, 22:45
 */

#define DRIVER_AUTHOR   "Harry Hornbacher <h.hornbacher@gmail.com>"
#define DRIVER_DESC     "Driver for 2x16 text LCD (4bit)"
#define DRIVER_NAME     "lcd1602"
#define DRV_VERSION     "0.0.1"

// timings
#define LCD_INIT_DELAY          500 //ms
#define LCD_STROBE_DELAY        2 //µs

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define PIN_DIR(handler,dir) gpio_set_one_pin_io_status(handler, dir, NULL);
#define PIN_SET(handler,val) gpio_write_one_pin_value(handler, val, NULL);
#define PIN_GET(handler) gpio_read_one_pin_value(handler, NULL);

struct lcd1602_data {
    unsigned pin_rw, pin_rs, pin_e;
    unsigned data[4];
};
