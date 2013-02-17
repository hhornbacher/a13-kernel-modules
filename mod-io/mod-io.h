/* 
 * File:   mod-io.h
 * Author: hhornbacher
 *
 * Created on 16. Februar 2013, 23:13
 */

#define DRIVER_AUTHOR   "Harry Hornbacher <h.hornbacher@gmail.com>"
#define DRIVER_DESC     "Driver for Olimex MOD-IO UEXT module"
#define DRIVER_NAME     "mod-io"
#define DRV_VERSION     "0.0.1"

#define MOD_IO_I2C_ADDR 0x58

#define PROC_ADC_1      0x1
#define PROC_ADC_2      0x2
#define PROC_ADC_3      0x3
#define PROC_ADC_4      0x4
#define PROC_DIO_1      0x11
#define PROC_DIO_2      0x12
#define PROC_DIO_3      0x13
#define PROC_DIO_4      0x14
#define PROC_RELAY_1    0x21
#define PROC_RELAY_2    0x22
#define PROC_RELAY_3    0x23
#define PROC_RELAY_4    0x24

#define RELAY_STATE(id) ((mod_io_relay_state & (1 << (id-1)))!=0)?1:0

static unsigned char mod_io_relay_state;

static void mod_io_ctrl_relay(int id, int state);
static int mod_io_read_dio(int id);
static int mod_io_read_adc(int id);

static struct i2c_driver mod_io_driver;
static struct i2c_client *this_client;

static void mod_io_init_i2c();
static void mod_io_exit_i2c();

static struct proc_dir_entry *mod_io_proc;
static struct proc_dir_entry *mod_io_proc_adc_dir;
static struct proc_dir_entry *mod_io_proc_dio_dir;
static struct proc_dir_entry *mod_io_proc_relay_dir;

static void mod_io_init_procfs();
static void mod_io_exit_procfs();
static int mod_io_proc_read(char* buffer, char** start, off_t off, int count, int* peof, void* data);
static int mod_io_proc_write(struct file* file, const char* buffer, unsigned long count, void* data);
