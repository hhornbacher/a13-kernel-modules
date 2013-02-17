/*
 *  mod-io.c
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>    
#include <linux/proc_fs.h>
#include <asm/uaccess.h> 
#include "mod-io.h"

static unsigned char mod_io_relay_state = 0;

/* Control relays */
static void mod_io_ctrl_relay(int id, int state) {
    int err;

    if (state == 1)
        mod_io_relay_state |= (1 << (id - 1));
    if (state == 0)
        mod_io_relay_state &= 0xf^(1 << (id - 1));

    unsigned char data[] = {0x10, mod_io_relay_state};
    err = i2c_master_send(this_client, data, sizeof (data));
}

/* Read digital IO values */
static int mod_io_read_dio(int id) {
    int err;

    unsigned char data[] = {0x20};
    unsigned char buffer[2];

    err = i2c_master_send(this_client, data, sizeof (data));
    err = i2c_master_recv(this_client, buffer, 1);

    if ((buffer[0]&(1 << (id - 1))) > 0)
        return 1;
    else
        return 0;
}

/* Read ADC values */
static int mod_io_read_adc(int id) {
    int i, temp = 0, err;

    unsigned char data[] = {0x30 + (id - 1)};
    unsigned char buffer[2];

    err = i2c_master_send(this_client, data, sizeof (data));
    err = i2c_master_recv(this_client, buffer, 2);

    for (i = 0; i < 8; i++) {
        temp |= ((data[0] & 0x80) ? 1 : 0) << i;
        data[0] <<= 1;

    }
    temp |= ((data[1] & 0x02) ? 1 : 0) << 8;
    temp |= ((data[1] & 0x01) ? 1 : 0) << 9;

    return temp;
}

static void mod_io_init_procfs() {
    struct proc_dir_entry *child;
    mod_io_proc = proc_mkdir("mod-io", NULL);
    mod_io_proc_adc_dir = proc_mkdir("adc", mod_io_proc);
    child = create_proc_read_entry("1", 0444, mod_io_proc_adc_dir, mod_io_proc_read, NULL);
    child->data = PROC_ADC_1;
    child = create_proc_read_entry("2", 0444, mod_io_proc_adc_dir, mod_io_proc_read, NULL);
    child->data = PROC_ADC_2;
    child = create_proc_read_entry("3", 0444, mod_io_proc_adc_dir, mod_io_proc_read, NULL);
    child->data = PROC_ADC_3;
    child = create_proc_read_entry("4", 0444, mod_io_proc_adc_dir, mod_io_proc_read, NULL);
    child->data = PROC_ADC_4;

    mod_io_proc_dio_dir = proc_mkdir("dio", mod_io_proc);
    child = create_proc_read_entry("1", 0444, mod_io_proc_dio_dir, mod_io_proc_read, NULL);
    child->data = PROC_DIO_1;
    child = create_proc_read_entry("2", 0444, mod_io_proc_dio_dir, mod_io_proc_read, NULL);
    child->data = PROC_DIO_2;
    child = create_proc_read_entry("3", 0444, mod_io_proc_dio_dir, mod_io_proc_read, NULL);
    child->data = PROC_DIO_3;
    child = create_proc_read_entry("4", 0444, mod_io_proc_dio_dir, mod_io_proc_read, NULL);
    child->data = PROC_DIO_4;

    mod_io_proc_relay_dir = proc_mkdir("relay", mod_io_proc);
    child = create_proc_entry("1", 0644, mod_io_proc_relay_dir);
    child->write_proc = mod_io_proc_write;
    child->read_proc = mod_io_proc_read;
    child->data = PROC_RELAY_1;
    child = create_proc_entry("2", 0644, mod_io_proc_relay_dir);
    child->write_proc = mod_io_proc_write;
    child->read_proc = mod_io_proc_read;
    child->data = PROC_RELAY_2;
    child = create_proc_entry("3", 0644, mod_io_proc_relay_dir);
    child->write_proc = mod_io_proc_write;
    child->read_proc = mod_io_proc_read;
    child->data = PROC_RELAY_3;
    child = create_proc_entry("4", 0644, mod_io_proc_relay_dir);
    child->write_proc = mod_io_proc_write;
    child->read_proc = mod_io_proc_read;
    child->data = PROC_RELAY_4;
}

static void mod_io_exit_procfs() {
    remove_proc_entry("1", mod_io_proc_adc_dir);
    remove_proc_entry("2", mod_io_proc_adc_dir);
    remove_proc_entry("3", mod_io_proc_adc_dir);
    remove_proc_entry("4", mod_io_proc_adc_dir);
    remove_proc_entry("1", mod_io_proc_dio_dir);
    remove_proc_entry("2", mod_io_proc_dio_dir);
    remove_proc_entry("3", mod_io_proc_dio_dir);
    remove_proc_entry("4", mod_io_proc_dio_dir);
    remove_proc_entry("1", mod_io_proc_relay_dir);
    remove_proc_entry("2", mod_io_proc_relay_dir);
    remove_proc_entry("3", mod_io_proc_relay_dir);
    remove_proc_entry("4", mod_io_proc_relay_dir);
    remove_proc_entry("relay", mod_io_proc);
    remove_proc_entry("dio", mod_io_proc);
    remove_proc_entry("adc", mod_io_proc);
    remove_proc_entry("mod_io", NULL);
}

static int mod_io_proc_read(char* buffer, char** start, off_t off, int count, int* peof, void* data) {
    int len;

    switch ((int) data) {
        case PROC_ADC_1:
            len = sprintf(buffer, "%d", mod_io_read_adc(1));
            break;
        case PROC_ADC_2:
            len = sprintf(buffer, "%d", mod_io_read_adc(2));
            break;
        case PROC_ADC_3:
            len = sprintf(buffer, "%d", mod_io_read_adc(3));
            break;
        case PROC_ADC_4:
            len = sprintf(buffer, "%d", mod_io_read_adc(4));
            break;
        case PROC_DIO_1:
            len = sprintf(buffer, "%d", mod_io_read_dio(1));
            break;
        case PROC_DIO_2:
            len = sprintf(buffer, "%d", mod_io_read_dio(2));
            break;
        case PROC_DIO_3:
            len = sprintf(buffer, "%d", mod_io_read_dio(3));
            break;
        case PROC_DIO_4:
            len = sprintf(buffer, "%d", mod_io_read_dio(4));
            break;
        case PROC_RELAY_1:
            len = sprintf(buffer, "%d", RELAY_STATE(1));
            break;
        case PROC_RELAY_2:
            len = sprintf(buffer, "%d", RELAY_STATE(2));
            break;
        case PROC_RELAY_3:
            len = sprintf(buffer, "%d", RELAY_STATE(3));
            break;
        case PROC_RELAY_4:
            len = sprintf(buffer, "%d", RELAY_STATE(4));
            break;
    }
    return len;
}

/* Write (Only for Relay*/
static int mod_io_proc_write(struct file* file, const char* buffer, unsigned long count, void* data) {
    // Get relay ID from filename
    int id = file->f_path.dentry->d_iname[0] - 48;
    char tmp[2];

    // Copy buffer to kernel space
    copy_from_user(tmp, buffer, 2);

    mod_io_ctrl_relay(id, tmp[0] - 48);

    return count;
}

/* Addresses to scan */
static unsigned short mod_io_address_list[] = {MOD_IO_I2C_ADDR, I2C_CLIENT_END};

/* Addresses to scan */
static struct i2c_driver mod_io_driver = {
    .driver =
    {
        .name = DRIVER_NAME,
    },
    .address_list = mod_io_address_list,
};

static void mod_io_init_i2c() {
    // Instatiating client device
    struct i2c_adapter *i2c_adap;
    struct i2c_board_info i2c_info;
    i2c_adap = i2c_get_adapter(2);
    memset(&i2c_info, 0, sizeof (struct i2c_board_info));
    strlcpy(i2c_info.type, "mod_io", I2C_NAME_SIZE);
    this_client = i2c_new_probed_device(i2c_adap, &i2c_info, mod_io_address_list, NULL);
    i2c_put_adapter(i2c_adap);
    i2c_add_driver(&mod_io_driver);
}

static void mod_io_exit_i2c() {
    i2c_unregister_device(this_client);
    i2c_del_driver(&mod_io_driver);
}

static int __init mod_io_init_driver(void) {
    mod_io_init_i2c();
    mod_io_init_procfs();

    // Set relay to neutral state
    mod_io_ctrl_relay(1, 0);
    mod_io_ctrl_relay(2, 0);
    mod_io_ctrl_relay(3, 0);
    mod_io_ctrl_relay(4, 0);

    printk(KERN_INFO "%s: Loaded i2c driver\n", __func__);
    return 0;
}

static void __exit mod_io_exit_driver(void) {
    mod_io_exit_i2c();
    mod_io_exit_procfs();
    printk(KERN_INFO "%s: Unloaded i2c driver\n", __func__);
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(mod_io_init_driver);
module_exit(mod_io_exit_driver);
