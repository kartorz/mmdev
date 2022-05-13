/*
 * The wine memory mananger api.
 *
 * 
 *
 * ================================================
 * Copyright (C) 2021 NFSChina. All rights reserved.
 * 
 * Author:
 *     Li Li Qiong <liqiong@nfschina.com>
 *     Yu Zhe <yuzhe@nfschina.com>
 *     Tu Ren Yu@nfschina.com <renyu@nfschina.com>
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/io.h>

#define WINE_MMDEV_MAJOR 333

MODULE_AUTHOR("liqiong <liqiong@nfschina>");
MODULE_LICENSE("GPL");

//#include "mmcch.c"

struct mmdev {
	unsigned long vm_size;
	struct kmem_cache *cache;
	struct cdev cdev;
};
static struct mmdev wine_mmdev;

static unsigned int mmdev_inc = 0;

static int mmdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct mmdev * mmdev = filp->private_data;

	unsigned long vm_start = vma->vm_start;
	unsigned long vm_end = vma->vm_end;
	unsigned long vm_size = vma->vm_end - vma->vm_start;

	unsigned long pfn;
	unsigned long len;
	int order;
	printk(KERN_INFO "[mmdev_mmap], range (0x%lx-->0x%lx) \n", vm_start, vm_end);
	if (vm_size == 0)
		goto exit;

	wine_mmdev = kmem_cache_alloc(wine_mmdev.cache, GFP_KERNEL);
	pfn = virt_to_phys(mmdev->cache) >> PAGE_SHIFT;
	if (remap_pfn_range(vma, vma->vm_start, pfn, vm_size, vma->vm_page_prot))
	{
		printk(KERN_ERR "remap page range failed\n");
		goto error;
	}
exit:
	return 0;

error:
	mmdev->vm_size = 0;
	return -ENOMEM;
}


static ssize_t mmdev_read(struct file *filp, char __user *buffer, size_t size , loff_t *ppos)
{
	struct mmdev *mmdev = filp->private_data;
	int ret;
	ssize_t n;
	unsigned int count = size;
	unsigned int p = *ppos;
	if (mmdev->vm_size - p > size)
		n = size;
	else
		n = mmdev->vm_size - p;
	ret = copy_to_user(buffer, mmdev->cache, n);
	if (ret != 0)
	{
		return -EFAULT;
	}

	*ppos += n;
	return n;
}

static ssize_t mmdev_write(struct file *filp, const char __user *buffer, size_t size , loff_t *ppos)
{
	struct mmdev *mmdev = filp->private_data;
	int ret;
	ssize_t n;
	unsigned int count = size;
	unsigned int p = *ppos;
	if (mmdev->vm_size - p > count)
		n = count;
	else
		n = mmdev->vm_size - p;
	ret = copy_from_user(mmdev->cache, buffer, n);
	if (ret != 0)
	{
		return -EFAULT;
	}

	return n;
}

static int mmdev_open(struct inode *inode, struct file *filp)
{
	if(mmdev_inc > 0) 
		return -ERESTARTSYS;

	mmdev_inc++;

	struct mmdev *mmdev = NULL;
	mmdev = container_of(inode->i_cdev, struct mmdev, cdev);
	filp->private_data = mmdev;
	return 0;
}

static int mmdev_release(struct inode *inode, struct file *filp)
{
	mmdev_inc--;
	return 0;
}

static const struct file_operations mmdev_fops = {
	.owner              = THIS_MODULE,
	.read		    = mmdev_read,
	.write		    = mmdev_write,
	.open		    = mmdev_open,
	.release	    = mmdev_release,
	.mmap               = mmdev_mmap,
};

static int setup_dev(struct mmdev *dev)
{
	dev_t devno = MKDEV(WINE_MMDEV_MAJOR, 0);
	cdev_init(&dev->cdev, &mmdev_fops);
	dev->cdev.ops = &mmdev_fops;
	return cdev_add(&dev->cdev, devno, 1);
}

static int __init wine_mmdev_init(void)
{
	memset(&wine_mmdev, 0, sizeof(struct mmdev));
	wine_mmdev.cache = kmem_cache_create("wine_mmdev cache", 70, 0, SLAB_HWCACHE_ALIGN, NULL);
	if(wine_mmdev.cache == NULL)
		return -1;
	if (setup_dev(&wine_mmdev)) {
                //dev_err(&wine_mmdev->dev, "no dev number available!\n");
		return -1;
	}
	return 0;
}

static void __exit wine_mmdev_exit(void)
{
	struct mmdev *mmdev = &wine_mmdev; 
	if(wine_mmdev.cache)
		kmem_cache_destroy(wine_mmdev.cache);
	cdev_del(&wine_mmdev.cdev);
}

module_init(wine_mmdev_init);
module_exit(wine_mmdev_exit);
