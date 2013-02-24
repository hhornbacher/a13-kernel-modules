/*
 *  lcd1602.c
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/delay.h>
#include <asm/delay.h>

#include <mach/sys_config.h>

#include "lcd1602.h"

static struct lcd1602_data *my_lcd_data;

static void lcd_set_data(unsigned char data) {
    if (data & 1) {
        PIN_SET(my_lcd_data->data[0], 1);
    } else {
        PIN_SET(my_lcd_data->data[0], 0);
    }

    if (data & 2) {
        PIN_SET(my_lcd_data->data[1], 1);
    } else {
        PIN_SET(my_lcd_data->data[1], 0);
    }

    if (data & 4) {
        PIN_SET(my_lcd_data->data[2], 1);
    } else {
        PIN_SET(my_lcd_data->data[2], 0);
    }

    if (data & 8) {
        PIN_SET(my_lcd_data->data[3], 1);
    } else {
        PIN_SET(my_lcd_data->data[3], 0);
    }
}

static void lcd_strobe() {
    PIN_SET(my_lcd_data->pin_e, 1);
    udelay(LCD_STROBE_DELAY);
    PIN_SET(my_lcd_data->pin_e, 0);
}

void lcd_wait_busy() {
    PIN_SET(my_lcd_data->pin_rw, 1);
    PIN_SET(my_lcd_data->pin_rs, 0);

    //D7 =Eingang
    PIN_DIR(my_lcd_data->data[3], 0);
    int busy;
    do {
        PIN_SET(my_lcd_data->pin_e, 1);
    udelay(LCD_STROBE_DELAY);
        busy = PIN_GET(my_lcd_data->data[3]);
        PIN_SET(my_lcd_data->pin_e, 0);

        lcd_strobe();
    } while (busy);

    //D7 = Ausgang
    PIN_DIR(my_lcd_data->data[3], 1);
}

static void lcd_send_nibble(unsigned char nibble) {
    lcd_set_data(nibble);
    lcd_strobe();
}

static void lcd_send_byte(unsigned char data) {
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data);
}

static void lcd_send_command(unsigned char cmd) {
    lcd_wait_busy();

    PIN_SET(my_lcd_data->pin_rw, 0);
    PIN_SET(my_lcd_data->pin_rs, 0);
    lcd_send_byte(cmd);
}

static void lcd_send_data(unsigned char cmd) {
    lcd_wait_busy();

    PIN_SET(my_lcd_data->pin_rw, 0);
    PIN_SET(my_lcd_data->pin_rs, 1);
    lcd_send_byte(cmd);
}

void lcd_print(char* string) {
	while(*string) {
		lcd_send_data(*string);
		string++;
	}
}

static void lcd_init() {
    mdelay(LCD_INIT_DELAY);

    PIN_SET(my_lcd_data->pin_e, 0);
    PIN_SET(my_lcd_data->pin_rw, 0);
    PIN_SET(my_lcd_data->pin_rs, 0);

    lcd_send_nibble(0x2); //4-Bit Mode (DL=0)
    mdelay(10);


    unsigned char cmd = 32;
    cmd |= 8;

    lcd_send_command(cmd); // 4-Bit Modus, Zeilen und Font setzen
    mdelay(10);
    //Display On/Off: 0 0 0 0 1 D C B
    lcd_send_command(0xC); //Display on, no cursor, no blink
    //Clear Display: 0 0 0 0 0 0 1
    lcd_send_command(0x1); //Clear Display and Cursor home
}

static int __init lcd1602_init_driver(void) {
    int err = 0;
    int used = 0;
    int i;
    char buffer[16];
    my_lcd_data = kzalloc(sizeof (struct lcd1602_data), GFP_KERNEL);

    err = script_parser_fetch("lcd1602_para", "lcd1602_used", &used, sizeof (used));
    if (err || used == 0) {
        printk(KERN_INFO "%s: Cannot setup lcd1602 driver, maybe not configured in script.bin?", __FUNCTION__);
    }

    // Get GPIO Pin config from script.bin
    my_lcd_data->pin_rw = gpio_request_ex("lcd1602_para", "lcd1602_rw");
    my_lcd_data->pin_rs = gpio_request_ex("lcd1602_para", "lcd1602_rs");
    my_lcd_data->pin_e = gpio_request_ex("lcd1602_para", "lcd1602_e");
    for (i = 0; i < 4; i++) {
        sprintf(buffer, "lcd1602_d%d", (i + 4));
        my_lcd_data->data[i] = gpio_request_ex("lcd1602_para", buffer);
    }
    /*
        if (!gpio_handler || err) {
            printk(KERN_INFO "%s: can not get \"dht22_para\" \"dht22_pin\" gpio handler, already used by others?", __FUNCTION__);
        
        }
     */
    lcd_init();
    lcd_print("Hallo Welt!");

    printk(KERN_INFO "%s: Loaded lcd1602 driver\n", __func__);
    return 0;
}

static void __exit lcd1602_exit_driver(void) {
    printk(KERN_INFO "%s: Unloaded lcd1602 driver\n", __func__);
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(lcd1602_init_driver);
module_exit(lcd1602_exit_driver);
