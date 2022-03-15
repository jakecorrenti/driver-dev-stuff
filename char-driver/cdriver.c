#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>

unsigned current_write_idx = 0;
unsigned string_len = 10;
char string[10] = {" "};

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
    unsigned i;
    pr_info("cdriver read\n");
    for (i = 0; i < string_len; i++)
    {
        if (copy_to_user(buf+i, string+i, 1))
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
    char buffer[1];
    size_t i;
    pr_info("cdriver write\n");

    if (copy_from_user(buffer, buf, 1))
    {
        pr_err("error copying to user\n");
        return -EFAULT;
    }

    if (current_write_idx >= string_len - 1)
        current_write_idx = 0;

    string[current_write_idx] = buffer[0];
    current_write_idx++;

    pr_info("buffer value: %c\n", buffer[0]);
    pr_info("current write index: %i\n", current_write_idx);

    pr_info("current string val: ");
    for (i = 0; i < string_len; i++)
    {
        pr_info("%c", string[i]);
    }

    *fpos += count;
    return count;
}

static loff_t cdriver_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t new_pos;

    switch (whence) {
        case 0: // SEEK_SET
            new_pos = offset;
            break;
        case 1: // SEEK_CUR 
            new_pos = filp->f_pos + offset;
            break;
        case 2: // SEEK_END
            new_pos = current_write_idx + offset;
            break;
        default: // cannot happen
            return -ENOTTY;
    }
    if (new_pos < 0 || new_pos >= string_len)
        return -EINVAL;

    filp->f_pos = new_pos;
    return new_pos;
}

static long cdriver_ioctl(struct file *filp, unsigned cmd, unsigned long arg)
{
    // TODO(jakecorrenti): have current_write_idx = 0 
    // TODO(jakecorrenti): change the direction in which the buffer vals are changed (front -> back | back -> front)
    return 3;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = cdriver_open,
    .release = cdriver_release,
    .read = cdriver_read,
    .write = cdriver_write,
    .llseek = cdriver_llseek,
    .unlocked_ioctl = cdriver_ioctl,
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
