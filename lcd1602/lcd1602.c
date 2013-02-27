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
#include "../gpio_makros.h"

static struct lcd1602_data *my_lcd_data;

inline void lcd_mode_read() {
    PIN_SET(my_lcd_data->pin_rw, 1);
    PIN_DIR(my_lcd_data->pin_d4, PIN_DIR_IN);
    PIN_DIR(my_lcd_data->pin_d5, PIN_DIR_IN);
    PIN_DIR(my_lcd_data->pin_d6, PIN_DIR_IN);
    PIN_DIR(my_lcd_data->pin_d7, PIN_DIR_IN);
}

inline void lcd_mode_write() {
    PIN_SET(my_lcd_data->pin_rw, 0);
    PIN_DIR(my_lcd_data->pin_d4, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d5, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d6, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d7, PIN_DIR_OUT);
}

inline void lcd_mode_data() {
    PIN_SET(my_lcd_data->pin_rs, 1);
}

inline void lcd_mode_command() {
    PIN_SET(my_lcd_data->pin_rs, 0);
}

static void lcd_set_data(unsigned char data) {
    if (data & 1) {
        PIN_SET(my_lcd_data->pin_d4, 1);
    } else {
        PIN_SET(my_lcd_data->pin_d4, 0);
    }

    if (data & 2) {
        PIN_SET(my_lcd_data->pin_d5, 1);
    } else {
        PIN_SET(my_lcd_data->pin_d5, 0);
    }

    if (data & 4) {
        PIN_SET(my_lcd_data->pin_d6, 1);
    } else {
        PIN_SET(my_lcd_data->pin_d6, 0);
    }

    if (data & 8) {
        PIN_SET(my_lcd_data->pin_d7, 1);
    } else {
        PIN_SET(my_lcd_data->pin_d7, 0);
    }
}

inline void lcd_strobe(int wait_us) {
    PIN_SET(my_lcd_data->pin_e, 1);

    if (wait_us < 1000)
        udelay(wait_us);
    else
        mdelay(wait_us / 1000);
    PIN_SET(my_lcd_data->pin_e, 0);
}

static void lcd_wait_busy() {
    printk(KERN_INFO "%s()", __FUNCTION__);
    udelay(30);
    lcd_mode_read();
    lcd_mode_command();
    lcd_strobe(2);
    int loop = 1;
    while (loop == 1) {
        loop = PIN_GET(my_lcd_data->pin_d7);
        lcd_strobe(2);
    }
}

static void lcd_send_nibble(unsigned char nibble) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_set_data(nibble);
    lcd_strobe(2);
}

static void lcd_send_byte(unsigned char data) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data);
    lcd_wait_busy();
}

static void lcd_send_command(u8 cmd) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_wait_busy();

    lcd_mode_write();
    lcd_mode_command();
    lcd_send_byte(cmd);
}

static void lcd_send_data(u8 data) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_wait_busy();

    lcd_mode_write();
    lcd_mode_data();
    lcd_send_byte(data);
}

inline void lcd_clear() {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_send_command(1);
}

inline void lcd_home() {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_send_command(2);
}

inline void lcd_entry_mode(u8 inc, u8 shift) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_send_command(4&(inc<<1)&shift);
}

/*
inline void lcd_set_ddram(u8 addr) {
    printk(KERN_INFO "%s()", __FUNCTION__);
    lcd_send_command(128&addr);
}
*/

static void lcd_write_str(char *str) {
	while(*str) {
		lcd_send_data(*str);
		str++;
	}
}

static void lcd_init() {
    printk(KERN_INFO "%s()", __FUNCTION__);
    
    // Init
    PIN_SET(my_lcd_data->pin_d4, 1);
    PIN_SET(my_lcd_data->pin_d5, 1);
    lcd_strobe(4000);
    lcd_strobe(100);
    lcd_strobe(100);
    
    lcd_wait_busy();
    
    lcd_clear();
    lcd_send_command(FUNCTION_SET | BIT_N); // 2 lines, 5x7 chars
    lcd_send_command(CMD_ONOFF | BIT_D); // Display off
    lcd_entry_mode(1,0);

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
    my_lcd_data->pin_d4 = gpio_request_ex("lcd1602_para", "lcd1602_d4");
    my_lcd_data->pin_d5 = gpio_request_ex("lcd1602_para", "lcd1602_d5");
    my_lcd_data->pin_d6 = gpio_request_ex("lcd1602_para", "lcd1602_d6");
    my_lcd_data->pin_d7 = gpio_request_ex("lcd1602_para", "lcd1602_d7");
    if (!my_lcd_data->pin_rw || !my_lcd_data->pin_rs || !my_lcd_data->pin_e || !my_lcd_data->pin_d4 || !my_lcd_data->pin_d5 || !my_lcd_data->pin_d6 || !my_lcd_data->pin_d7) {
        printk(KERN_INFO "%s: can not get all needed GPIO pins, maybe already used by others?", __FUNCTION__);
        return -1;
    }

    PIN_DIR(my_lcd_data->pin_rw, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_rs, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_e, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d4, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d5, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d6, PIN_DIR_OUT);
    PIN_DIR(my_lcd_data->pin_d7, PIN_DIR_OUT);

    PIN_SET(my_lcd_data->pin_rw, 0);
    PIN_SET(my_lcd_data->pin_rs, 0);
    PIN_SET(my_lcd_data->pin_e, 0);
    PIN_SET(my_lcd_data->pin_d4, 0);
    PIN_SET(my_lcd_data->pin_d5, 0);
    PIN_SET(my_lcd_data->pin_d6, 0);
    PIN_SET(my_lcd_data->pin_d7, 0);

    lcd_init();
    
    
    lcd_write_str("Hallo Welt!");

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
