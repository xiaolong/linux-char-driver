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
	{ 0x927fe8c4, "module_layout" },
	{ 0xa90c928a, "param_ops_int" },
	{ 0x37a0cba, "kfree" },
	{ 0x4530655b, "cdev_del" },
	{ 0xd51fa5c0, "cdev_add" },
	{ 0xc997a8cd, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xbda3498d, "kmem_cache_alloc_trace" },
	{ 0x2f3029e8, "kmalloc_caches" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x50eedeb8, "printk" },
	{ 0x57b09822, "up" },
	{ 0x670c0597, "down_interruptible" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9F07A5DA2F942B69B03F70C");
