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

#define WINE_MMDEV_MAJOR 333

MODULE_AUTHOR("liqiong <liqiong@nfschina>");
MODULE_LICENSE("GPL");

#include "mmcch.c"

struct mmdev {
	unsigned long vm_size;
	struct cdev cdev;
};
static struct mmdev wine_mmdev;

static int mmdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct mmdev * mm = filp->private_data;

	unsigned long vm_start = vma->vm_start;
	unsigned long vm_end = vma->vm_end;
	unsigned long vm_size = vma->vm_end - vma->vm_start;

	unsigned long pfn;
	unsigned long len;
	int order;
	struct page *pg;
	printk(KERN_INFO "[mmdev_mmap], range (0x%lx-->0x%lx) \n", vm_start, vm_end);
	if (vm_size == 0)
		goto exit;

        void * p = alloc_cache(vm_size);
        unsigned long pfn = virt_to_phys(p) >> PAGE_SHIFT;
        if (remap_pfn_range(vma, vma->vm_start, pfn, vm_size, vma->vm_page_prot)) {
		printk(KERN_ERR "remap page range failed\n");
		goto error;
	}
exit:
	return 0;

error:
	mmdev->vm_size = 0;
	return -ENOMEM;
}

static const struct file_operations mmdev_fops = {
	.owner              = THIS_MODULE,
	.read		    = mmdev_read,
	.write		    = mmdev_write,
	.open		    = mmdev_open,
	.release	    = mmdev_release,
	.mmap               = mmdev_mmap,
	.unlocked_ioctl	    = mmdev_ioctl,
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
	if (setup_dev(&wine_mmdev)) {
                dev_err(&wine_mmdev->dev, "no dev number available!\n");
		return -1;
	}
	return 0;
}

static void __exit wine_mmdev_exit(void)
{
	struct mmdev *mmdev = &wine_mmdev; 
	cdev_del(&wine_mmdev.cdev);
}

module_init(wine_mmdev_init);
module_exit(wine_mmdev_exit);
