#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>

static dev_t dev;
static struct cdev *cdev;

static int cdriver_open(struct inode *inode, struct file *filp)
{
    pr_info("cdriver open\n");
    return 0;
}

static int cdriver_release(struct inode *inode, struct file *filp)
{
    pr_info("cdriver release\n");
    return 0;
}

static ssize_t cdriver_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
    // currently just writes an endless amount of 0's and 1's to the screen
    unsigned i;
    pr_info("cdriver read\n");
    for (i = 0; i < count; i++)
    {
        if (copy_to_user(buf+i, i % 2 == 0 ? "0" : "1", 1))
        {
            pr_err("error copying to user\n");
            return -EFAULT;
        }
    }

    *fpos += count;
    return count;
}

static ssize_t cdriver_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
    pr_info("cdriver write\n");

    *fpos += count;
    return count;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = cdriver_open,
    .release = cdriver_release,
    .read = cdriver_read,
    .write = cdriver_write,
};

static int __init cdriver_init(void)
{
    int ret;
    pr_info("cdriver init\n");

    ret = alloc_chrdev_region(&dev, 0, 1, "cdriver");
    if (ret < 0)
    {
        pr_info("unable to register major/minor number for device");
        goto fail_alloc_dev_number;
    }

    pr_info("Major: %i minor: %i\n", MAJOR(dev), MINOR(dev));

    cdev = cdev_alloc();
    if (!cdev)
    {
        ret = -ENOMEM;
        pr_info("unable to allocate a character device for the system\n");
        goto fail_alloc_cdev;
    }

    cdev_init(cdev, &fops);

    ret = cdev_add(cdev, dev, 1);
    if (ret < 0)
    {
        pr_info("unable to add the character device to the system\n");
        goto fail_add_cdev;
    }

    return 0;

fail_add_cdev:
    cdev_del(cdev);
fail_alloc_cdev:
    unregister_chrdev_region(dev, 1);
fail_alloc_dev_number:
    return ret;
}

static void __exit cdriver_exit(void)
{
    pr_info("cdriver exit\n");
    cdev_del(cdev);
    unregister_chrdev_region(dev, 1);
}

MODULE_LICENSE("GPL");

module_init(cdriver_init);
module_exit(cdriver_exit);
