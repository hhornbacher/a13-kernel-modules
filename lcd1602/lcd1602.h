/*
 * lcd1602 - Driver for HD44780 compatible 16 x 2 display (4bit mode)
 *
 * Copyright (C) 2013 Harry Hornbacher <h.hornbacher@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#define DRIVER_AUTHOR   "Harry Hornbacher <h.hornbacher@gmail.com>"
#define DRIVER_DESC     "Driver for HD44780 compatible 16 x 2 display (4bit mode)"
#define DRIVER_NAME     "lcd1602"
#define DRV_VERSION     "0.0.1"

#define LCD_LINE1	0x00 //DDRAM line 1 addr 
#define LCD_LINE2	0x40 //DDRAM line 2 addr 

#define FUNCTION_SET    	32 	//Function Set
#define BIT_DL			16
#define BIT_N			8
#define BIT_F			4

#define CMD_ONOFF		8 //Display On/Off Command
#define BIT_D			4
#define BIT_C			2
#define BIT_B			1

struct lcd1602_data {
    unsigned pin_rw, pin_rs, pin_e, pin_d7, pin_d6, pin_d5, pin_d4;
};
