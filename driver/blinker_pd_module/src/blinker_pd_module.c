/*
 ============================================================================
 Name        : blinker_pd_module.c
 Author      : Nicolas Schuck
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/clk.h>

#include "hps.h"
#include "hps_0.h"
#include "blinker_module.h"

/* global variables */
static struct platform_driver blinker_platform_driver;
static void *blinker_map_mem;
static int g_blinker_driver_base_addr;
static int g_blinker_driver_size;
static unsigned long g_blinker_driver_clk_rate;

ssize_t leds_show(struct device_driver *drv, char *buf)
{
    u8 data;
    data = ioread8(IOADDR_BLINKER_LEDS(blinker_map_mem));
    pr_info("Leds position: %d\n, data");
    return scnprintf(buf, PAGE_SIZE, "%u\n", data);
}

ssize_t state_show(struct device_driver *drv, char *buf)
{
    u8 data;
    data = ioread8(IOADDR_BLINKER_STATE(blinker_map_mem));
    pr_info("State of LEDs: %d\n, data");
    return scnprintf(buf, PAGE_SIZE, "%u\n", data);
}

ssize_t config_store(struct device_driver *drv, const char *buf, size_t count)
{
    u8 data;

    if(buf == NULL)
    {
        pr_err("Error, string must not be NULL\n");
        return -EINVAL;
    }

    if(kstrtou8(buf, 10, &data) < 0)
    {
        pr_err("Could not convert string to interger\n");
        return -EINVAL;
    }

    if(data > 3 && data < 1)
    {
        pr_err("Invalid configuration data %d\n", data);
        return -EINVAL;
    }

    iowrite8(data, IOADDR_BLINKER_CONFIG(blinker_map_mem));
    pr_info("New configuration value: %d\n", data);

    return count;
}

ssize_t speed_store(struct device_driver *drv, const char *buf, size_t count)
{
    u8 data;

    if(buf == NULL)
    {
        pr_err("Error, string musst not be NULL\n");
        return -EINVAL;
    }

    if(kstrtou8(buf, 10, &data) < 0)
    {
      pr_err("Could not convert string to interger\n");
      return -EINVAL;
    }

    if(data > 3 && data < 1)
    {
      pr_err("Invalid configuration data %d\n", data);
      return -EINVAL;
    }

    iowrite8(data, IOADDR_BLINKER_SPEED(blinker_map_mem));
    pr_info("New speed value: %d \n", data);

    return count;
}

static DRIVER_ATTR(config, S_IWUSR|S_IWGRP, NULL, config_store);
static DRIVER_ATTR(leds, S_IRUSR|S_IRGRP, leds_show, NULL);
static DRIVER_ATTR(state, S_IRUSR|S_IRGRP, state_show, NULL);
static DRIVER_ATTR(speed, S_IRUSR|S_IRGRP, NULL, speed_store);

static struct of_device_id blinker_driver_dt_ids[] = {{ .compatible = "blinker,driver-1.0"}, { /* end of table*/ }};

MODULE_DEVICE_TABLE(of, blinker_driver_dt_ids);

static int blinker_probe(struct platform_device *pdev)
{
    int ret = -EINVAL;
    struct resource *res;
    struct resource *blinker_driver_mem_region;
    struct clk *clk;
    unsigned long clk_rate;

    /* get the memory region allocated by blinker device */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    if (res == NULL)
    {
        pr_err("IORESOURCE_MEM, 0 does not exist\n");
        goto bad_exit_return;
    }

    g_blinker_driver_base_addr = res->start;
    g_blinker_driver_size = resource_size(res);

    /* get clock resource */
    clk = clk_get(&pdev->dev, NULL);
    if(IS_ERR(clk))
    {
        pr_err("clk not available\n");
        goto bad_exit_return;
    }
    else
    {
        clk_rate = clk_get_rate(clk);
    }

    g_blinker_driver_clk_rate = clk_rate;

    /* Create sysfiles entries */
    ret = driver_create_file(&blinker_platform_driver.driver, &driver_attr_leds);
    if(ret < 0)
    {
        pr_err("failed to create leds sysf entry.");
        goto bad_exit_create_leds_file;
    }


    blinker_driver_mem_region = request_mem_region(g_blinker_driver_base_addr, g_blinker_driver_size, "blinker");

    if(blinker_driver_mem_region == NULL)
    {
        pr_err("request_mem_region failed.");
        goto bad_exit_request_mem;
        ret = -EBUSY;
    }

    blinker_map_mem = ioremap(g_blinker_driver_base_addr, g_blinker_driver_size);

    if(blinker_map_mem == NULL)
    {
        pr_err("ioremap failed.");
        goto bad_exit_ioremap;
        ret = -EFAULT;
    }

    /* All the operations were successfully develop */
    pr_info("blinker device successfully connected\n");
    return 0;


    /* Some kind of error occurred during the process of the module insertion */

    bad_exit_ioremap:
        release_mem_region(g_blinker_driver_base_addr, g_blinker_driver_size);

    bad_exit_request_mem:
        driver_remove_file(&blinker_platform_driver.driver, &driver_attr_state);

    bad_exit_create_state_file:
        driver_remove_file(&blinker_platform_driver.driver, &driver_attr_config);

    bad_exit_create_config_file:
        driver_remove_file(&blinker_platform_driver.driver, &driver_attr_leds);

    bad_exit_create_leds_file:
        driver_remove_file(&blinker_platform_driver.driver, &driver_attr_speed);

    bad_exit_return:
        pr_err("blinker device connect FAILED");
        return ret;
}

static int blinker_remove(struct platform_device *pdev)
{
    /* Sysfiles entries removal */
    driver_remove_file(&blinker_platform_driver.driver, &driver_attr_speed);
    driver_remove_file(&blinker_platform_driver.driver, &driver_attr_leds);
    driver_remove_file(&blinker_platform_driver.driver, &driver_attr_config);
    driver_remove_file(&blinker_platform_driver.driver, &driver_attr_state);

    /* Deallocate resources */
    release_mem_region(g_blinker_driver_base_addr, g_blinker_driver_size);
    iounmap(blinker_map_mem);

    /* Removal operations successfully done */
    pr_info("blinker device successfully removed\n");

    return 0;
}

static struct platform_driver blinker_platform_driver = {
        .probe = blinker_probe,
        .remove = blinker_remove,
        .driver = {
                    .name = "blinker",
                    .owner = THIS_MODULE,
                    .of_match_table = blinker_driver_dt_ids,
                  },
};

static int __init blinker_init(void)
{
    int ret;
    g_blinker_driver_base_addr = BLINKER_BASE;
    g_blinker_driver_size = BLINKER_SIZE;

    ret = platform_driver_register(&blinker_platform_driver);
    if(ret != 0)
    {
        pr_err("platform_driver_register returned %d\n", ret);
        goto bad_exit_driver_register;
    }

    pr_info("blinker module successfully inserted\n");
    return 0;

bad_exit_driver_register:
    pr_err("blinker module instert FAILED");
    return ret;
}

static void __exit blinker_exit(void)
{
    platform_driver_unregister(&blinker_platform_driver);
    pr_info("blinker driver successfully removed\n");
}

module_init(blinker_init);
module_exit(blinker_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio Carpeno <antionio.cruiz@upm.es>");
MODULE_AUTHOR("Mariano Ruiz <mariano.ruiz@upm.es>");
MODULE_DESCRIPTION("blinker peripheral platform_driver example");
MODULE_VERSION("1.0");
