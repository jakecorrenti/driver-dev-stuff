#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

static dev_t hello_dev;
static struct cdev *hello_cdev;

static int hello_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Hello module: open\n");
	return 0;
}

static int hello_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Hello module: release\n");
	return 0;
}

static ssize_t hello_read(struct file *file, char __user *data, size_t count, loff_t *fpos) {
	int ret;
	size_t i;
	const char str[] = "Hello";
	printk(KERN_INFO "Hello module: read\n");

	// infinite hellos
	for (i = 0; i < count; ++i) {
		ret = copy_to_user(data + i, str + (i % 5), 1);
		if (ret) {
			printk(KERN_ERR "Hello module: bad read! %i\n", ret);
			return ret;
		}
	}

	*fpos += count;
	return count;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = hello_open,
	.release = hello_release,
	.read = hello_read,
};

static int hello_init(void) {
	int ret;
	printk(KERN_ALERT "hello module: hello init\n");

	ret = alloc_chrdev_region(&hello_dev, 0, 1, "hello");
	if (ret < 0) {
		printk(KERN_ERR "Hello module: Unable to alloc char dev region %i\n", ret);
		goto fail_region;
	}

	hello_cdev = cdev_alloc();
	if (!hello_cdev) {
		ret = -ENOMEM;
		printk(KERN_ERR "Hello module: unable to allocate memoru for cdev %i\n", ret);
		goto fail_cdev;
	}

	cdev_init(hello_cdev, &fops);

	ret = cdev_add(hello_cdev, hello_dev, 1);
	if (ret < 0) {
		printk(KERN_ERR "Hello module: unable to add char dev %i\n", ret);
		goto fail_add;
	}

	return 0;

fail_add:
	cdev_del(hello_cdev);
fail_cdev:
	unregister_chrdev_region(hello_dev, 1);
fail_region:
	return ret;
}

static void hello_exit(void) {
	printk(KERN_ALERT "Hello module: hello exit\n");
	cdev_del(hello_cdev);
	unregister_chrdev_region(hello_dev, 1);
}

module_init(hello_init);
module_exit(hello_exit);
