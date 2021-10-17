#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/platform_device.h>
#include<linux/uaccess.h>
#include<linux/ioport.h>
#include<linux/io.h>
#include<linux/clk.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/ioctl.h>
#include<linux/interrupt.h>
#include<linux/wait.h>
#include<linux/sched.h>
#include"blinker_module.h"

/* global variables */
static struct semaphore g_dev_probe_sem;
static int g_platform_probe_flag;
static void *blinker_map_mem;
static int g_blinker_driver_base_addr;
static int g_blinker_driver_size;
static unsigned long g_blinker_driver_clk_rate;
static int g_blinker_driver_irq;
static uint32_t g_irq_count;
static spinlock_t g_irq_lock;
static wait_queue_head_t g_irq_wait_queue;
static int interrupt_flag;
static struct platform_driver blinker_platform_driver;
/******************************************************************************
MISC DEVICE DRIVER
******************************************************************************/
struct blinker_dev {
    struct semaphore sem;
    unsigned long open_count;
    unsigned long release_count;
    unsigned long ioctl_count;
    unsigned long read_count;
    unsigned long write_count;
};

static struct blinker_dev the_blinker_dev = {
    .open_count = 0,
    .release_count = 0,
    .ioctl_count = 0,
    .write_count = 0,
    .read_count = 0,
};

static int blinker_dev_open(struct inode *ip, struct file *fp)
{
    struct blinker_dev *dev = &the_blinker_dev;

    if (down_interruptible(&dev->sem))
    {
        pr_info("blinker_dev_open sem interrupted exit\n");
        return -ERESTARTSYS;
    }

    fp->private_data = dev;
    dev->open_count++;
    pr_info("open_count: 0x%08lX\n", dev->open_count);
    up(&dev->sem);

    return 0;
}

static int blinker_dev_release(struct inode *ip, struct file *fp)
{
    struct blinker_dev *dev = fp->private_data;

    if (down_interruptible(&dev->sem))
    {
        pr_info("blinker_dev_open sem interrupted exit\n");

        return -ERESTARTSYS;
    }
    dev->release_count++;
    up(&dev->sem);
    pr_info("release_count: 0x%08lX\n", dev->release_count);

    return 0;
}


static long blinker_dev_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    struct blinker_dev *dev = fp->private_data;

    u8 speed;
    u8 current_config;
    u8 mode;
    u8 leds;

    unsigned long flags;

    if (down_interruptible(&dev->sem))
    {
        pr_info("blinker_dev_open sem interrupted exit\n");
        return -ERESTARTSYS;
    }

    dev->ioctl_count++;

    switch (cmd)
    {
        case IOC_SET_SPEED:
            if (get_user(speed, (uint32_t *)arg) < 0)
            {
                up(&dev->sem);
                pr_info("blinker_dev_ioctl get_user exit.\n");

                return -EFAULT;
            }

            if (speed < 1 || speed > 15)
            {
                up(&dev->sem);
                pr_err("Invalid speed value %d\n", speed);

                return -EINVAL;
            }

            /* acquire the irq_lock */
            spin_lock_irqsave(&g_irq_lock, flags);
            iowrite8(speed, IOADDR_BLINKER_SPEED(blinker_map_mem));

            /* release the irq_lock */
            spin_unlock_irqrestore(&g_irq_lock, flags);

            break;

        case IOC_SET_RUNNING:
            /* acquire the irq_lock */
            spin_lock_irqsave(&g_irq_lock, flags);
            current_config = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));
            iowrite8((current_config & BLINKER_CONFIG_MSK) | BLINKER_START_MSK, IOADDR_BLINKER_CONFIG(blinker_map_mem));

            /* release the irq_lock */
            spin_unlock_irqrestore(&g_irq_lock, flags);

            break;

        case IOC_CLEAR_RUNNING:
            /* acquire the irq_lock */
            spin_lock_irqsave(&g_irq_lock, flags);
            current_config = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));
            iowrite8((current_config & BLINKER_CONFIG_MSK) & BLINKER_STOP_MSK, IOADDR_BLINKER_CONFIG(blinker_map_mem));

            /* release the irq_lock */
            spin_unlock_irqrestore(&g_irq_lock, flags);

            break;

        case IOC_ENA_INT:
            /* acquire the irq_lock */
            spin_lock_irqsave(&g_irq_lock, flags);
            current_config = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));
            iowrite8((current_config & BLINKER_CONFIG_MSK) | BLINKER_IENA_MSK, IOADDR_BLINKER_CONFIG(blinker_map_mem));

            /* release the irq_lock */
            spin_unlock_irqrestore(&g_irq_lock, flags);

            break;

        case IOC_DIS_INT:
            /* acquire the irq_lock */
            spin_lock_irqsave(&g_irq_lock, flags);
            current_config = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));
            iowrite8((current_config & BLINKER_CONFIG_MSK) & BLINKER_IDIS_MSK, IOADDR_BLINKER_CONFIG(blinker_map_mem));

            /* release the irq_lock */
            spin_unlock_irqrestore(&g_irq_lock, flags);

            break;

        case IOC_SET_MODE:
            if (get_user(mode, (uint32_t *)arg) < 0)
            {
                up(&dev->sem);
                pr_info("blinker_dev_ioctl get_user exit.\n");

                return -EFAULT;
            }

            if (mode < 0 || mode > 1)
            {
                up(&dev->sem);
                pr_err("Invalid mode value %d\n", mode);

                return -EINVAL;
            }

            /* acquire the irq_lock */
            spin_lock_irqsave(&g_irq_lock, flags);
            current_config = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));

            if (mode == 0)
            {
                iowrite8((current_config & BLINKER_CONFIG_MSK) & BLINKER_SHIFT_MSK, IOADDR_BLINKER_CONFIG(blinker_map_mem));
            }

            else
            {
                iowrite8((current_config & BLINKER_CONFIG_MSK) | BLINKER_BLINK_MSK,
                        IOADDR_BLINKER_CONFIG(blinker_map_mem));
            }

            /* release the irq_lock */
            spin_unlock_irqrestore(&g_irq_lock, flags);

            break;

        case IOC_GET_LEDS:
            leds = ioread8(IOADDR_BLINKER_LEDS(blinker_map_mem));

            if (put_user(leds, (uint32_t *)arg) < 0)
            {
                up(&dev->sem);
                pr_info("blinker_dev_ioctl put_user exit\n");

                return -EFAULT;
            }

            break;

        case IOC_GET_CONFIG:
            current_config = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));
            if (put_user(current_config, (uint32_t *)arg) < 0)
            {
                up(&dev->sem);
                pr_info("blinker_dev_ioctl put_user exit\n");

                return -EFAULT;
            }

            break;

        default:
            up(&dev->sem);
            pr_info("blinker_dev_ioctl bad cmd exit\n");

            return -EINVAL;
    }

    up(&dev->sem);
    pr_info("ioctl_count: 0x%08lX\n", dev->ioctl_count);

    return 0;
}

static ssize_t blinker_dev_read(struct file *fp, char __user *user_buffer, size_t count, loff_t *offset)
{
    struct blinker_dev *dev = fp->private_data;
    unsigned long flags;

    u8 state;

    if (down_interruptible(&dev->sem))
    {
        pr_info("blinker_dev_read sem interrupted exit\n");

        return -ERESTARTSYS;
    }

    dev->read_count++;

    if (wait_event_interruptible(g_irq_wait_queue, interrupt_flag != 0))
    {
        up(&dev->sem);
        pr_info("blinker_dev_read wait interrupted exit\n");

        return -ERESTARTSYS;
    }

    /* acquire the irq_lock */
    spin_lock_irqsave(&g_irq_lock, flags);
    interrupt_flag = 0;

    /* release the irq_lock */
    spin_unlock_irqrestore(&g_irq_lock, flags);
    memcpy_fromio(&state, IOADDR_BLINKER_STATE(blinker_map_mem), 1);

    if (copy_to_user(user_buffer, &state, 1))
    {
        up(&dev->sem);
        pr_info("blinker_dev_read copy_to_user exit\n");

        return -EFAULT;
    }

    up(&dev->sem);
    pr_info("read_count: 0x%08lX\n", dev->read_count);

    return count;
}

static ssize_t blinker_dev_write(struct file *fp, const char __user *user_buffer, size_t count, loff_t *offset)
{
    struct blinker_dev *dev = fp->private_data;

    u8 config;

    if (down_interruptible(&dev->sem))
    {
        pr_info("blinker_dev_write sem interrupted exit\n");

        return -ERESTARTSYS;
    }

    dev->write_count++;

    if (copy_from_user(&config, user_buffer, 1))
    {
        up(&dev->sem);
        pr_info("blinker_dev_write copy_from_user exit\n");

        return -EFAULT;
    }

    if (config < 1 || config > 7)
    {
        up(&dev->sem);
        pr_err("Invalid configuration value %d\n", config);

        return -EINVAL;
    }

    memcpy_toio(IOADDR_BLINKER_CONFIG(blinker_map_mem), &config, 1);
    up(&dev->sem);
    pr_info("write_count: 0x%08lX\n", dev->write_count);

    return count;
}

static const struct file_operations blinker_dev_fops = {
    .owner = THIS_MODULE,
    .open = blinker_dev_open,
    .release = blinker_dev_release,
    .unlocked_ioctl = blinker_dev_ioctl,
    .read = blinker_dev_read,
    .write = blinker_dev_write,
};

static struct miscdevice blinker_dev_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "blinker_misc",
    .fops = &blinker_dev_fops,
};

/******************************************************************************
IRQ HANDLER, SHOW AND STORE METHODS
******************************************************************************/

irqreturn_t blinker_driver_interrupt_handler(int irq, void *dev_id)
{
    u8 data;
    spin_lock(&g_irq_lock);

    /* clear the interrupt by reading the state register*/
    data = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));

    /* increment the IRQ count */
    g_irq_count++;
    pr_info("IRQ handler executed. irq_count: 0x%08X\n", g_irq_count);
    data = ioread8(IOADDR_BLINKER_LEDS(blinker_map_mem));
    interrupt_flag = 1;
    spin_unlock(&g_irq_lock);
    wake_up_interruptible(&g_irq_wait_queue);

    return IRQ_HANDLED;
}

ssize_t leds_show(struct device_driver *drv, char *buf)
{
    ----------------------------
}

ssize_t state_show(struct device_driver *drv, char *buf)
{
    ----------------------------
}

ssize_t config_store(struct device_driver *drv, const char *buf, size_t count)
{
    ----------------------------

    /* acquire the irq_lock */
    spin_lock_irqsave(&g_irq_lock, flags);
    iowrite8(data, IOADDR_BLINKER_CONFIG(blinker_map_mem));

    /* release the irq_lock */
    spin_unlock_irqrestore(&g_irq_lock, flags);

    ----------------------------
}

ssize_t speed_store(struct device_driver *drv, const char *buf, size_t count)
{
    ----------------------------
}

static DRIVER_ATTR(speed, S_IWUGO, NULL, speed_store);
static DRIVER_ATTR(config, S_IWUGO, NULL, config_store);
static DRIVER_ATTR(leds, S_IRUGO, leds_show, NULL);
static DRIVER_ATTR(state, S_IRUGO, state_show, NULL);

static int blinker_probe(struct platform_device *pdev)
{
    ----------------------------

    int irq;

    /* acquire the probe lock */
    if (down_interruptible(&g_dev_probe_sem)) { return -ERESTARTSYS; }

    /* check that any other device is using the driver */
    if (g_platform_probe_flag != 0) { goto bad_exit_return; }

    /* get blinker memory resource */
    ----------------------------

    /* get our interrupt resource */
    irq = platform_get_irq(pdev, 0);
    if (irq < 0)
    {
        pr_err("irq not available\n");
        goto bad_exit_return;
    }

    g_blinker_driver_irq = irq;

    /* get blinker clock resource */
    ----------------------------

    /* register our interrupt handler */
    init_waitqueue_head(&g_irq_wait_queue);
    spin_lock_init(&g_irq_lock);
    g_irq_count = 0;
    interrupt_flag = 0;
    ret = request_irq(  g_blinker_driver_irq,
                        blinker_driver_interrupt_handler,
                        0,
                        blinker_platform_driver.driver.name,
                        &blinker_platform_driver);

    if (ret)
    {
        pr_err("request_irq failed");
        goto bad_exit_return;
    }

    /* create the sysfs entries */
    ----------------------------

    /* reserve blinker memory region */
    ----------------------------

    /* ioremap blinker memory region */
    ----------------------------

    /* register misc device blinker */
    sema_init(&the_blinker_dev.sem, 1);
    ret = misc_register(&blinker_dev_device);

    if (ret != 0)
    {
        pr_warn("Could not register misc device \"blinker_misc\"...");
        goto bad_exit_register_misc_device;
    }

    /* mark the driver as being used*/
    g_platform_probe_flag = 1;

    /* release the semaphore */
    up(&g_dev_probe_sem);
    pr_info("blinker device successfully connected\n");

    return 0;

    /* Exit with errors release the blocked resources */
    ----------------------------
bad_exit_freeirq:
    free_irq(g_blinker_driver_irq, &blinker_platform_driver);

    /* release the semaphore */
    up(&g_dev_probe_sem);
    pr_err("blinker device connect FAILED");

    return ret;
}

tatic int blinker_remove(struct platform_device *pdev)
{
    free_irq(g_blinker_driver_irq, &blinker_platform_driver);
    misc_deregister(&blinker_dev_device);

    /* remove sysfs entries and release memory resources */
    ----------------------------

    if (down_interruptible(&g_dev_probe_sem)) { return -ERESTARTSYS; }

    /* mark the driver as free*/
    g_platform_probe_flag = 0;
    up(&g_dev_probe_sem);

    ----------------------------
}

static struct platform_driver blinker_platform_driver = {
    .probe = blinker_probe,
    .remove = blinker_remove,
    .driver = {
        .name = "blinker_pd",
        .owner = THIS_MODULE,
        .of_match_table = blinker_driver_dt_ids,
    },
};

static int __init blinker_init(void)
{
    ----------------------------

    sema_init(&g_dev_probe_sem, 1);

    ----------------------------
}
static void __exit blinker_exit(void)
{
    ----------------------------
}

module_init(blinker_init);
module_exit(blinker_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio Carpeño <antonio.cruiz@upm.es>");
MODULE_AUTHOR("Mariano Ruiz <mariano.ruiz@upm.es>");
MODULE_DESCRIPTION("blinker peripheral misc_platform_driver example");
MODULE_VERSION("1.0");
