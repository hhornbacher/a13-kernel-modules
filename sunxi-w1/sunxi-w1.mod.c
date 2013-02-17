#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xcc490480, "module_layout" },
	{ 0x42a28861, "platform_driver_unregister" },
	{ 0x1b902a81, "platform_driver_probe" },
	{ 0xbdf94507, "gpio_read_one_pin_value" },
	{ 0x20fd0cbb, "gpio_write_one_pin_value" },
	{ 0x3aad3729, "kmalloc_caches" },
	{ 0xd65d0b87, "dev_set_drvdata" },
	{ 0x5fa32a89, "gpio_set_one_pin_io_status" },
	{ 0x383e8a27, "gpio_request_ex" },
	{ 0x84c41780, "script_parser_fetch" },
	{ 0x8e352198, "kmem_cache_alloc" },
	{ 0x27e1a049, "printk" },
	{ 0x37a0cba, "kfree" },
	{ 0x344a50ed, "gpio_release" },
	{ 0x270a2355, "dev_get_drvdata" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B50766D11461CD4257BF7D6");
