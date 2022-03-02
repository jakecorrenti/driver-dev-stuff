#include <linux/init.h>
#include <linux/module.h>

static int __init driver_init(void) {
  printk(KERN_ALERT "module init");
  return 0;
}

static void __exit driver_exit(void) { printk(KERN_ALERT "module exit"); }

MODULE_LICENSE("GPL")
MODULE_AUTHOR("Jake Correnti")

module_init(driver_init);
module_exit(driver_exit);
