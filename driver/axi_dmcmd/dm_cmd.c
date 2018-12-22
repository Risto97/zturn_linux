#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/io.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>

#include "img.hpp"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DM_CMD driver");
#define DEVICE_NAME "dm_cmd"
#define DRIVER_NAME "dm_cmd"

#define DATAMOVERCMD_BASEADDR 0x43C00000
#define DATAMOVERCMD_HIGHADDR 0x43C0FFFF

#define DATAMOVERCMD_MM2S_ADDR_OFFSET	  0x00
#define DATAMOVERCMD_MM2S_BTT_OFFSET		0x04
#define DATAMOVERCMD_S2MM_ADDR_OFFSET	  0x08
#define DATAMOVERCMD_S2MM_BTT_OFFSET		0x0C
#define DATAMOVERCMD_START_CMD_OFFSET		0x10

#define IMG_WIDTH 320
#define IMG_HEIGHT 240
#define IMG_BUFF_SIZE IMG_WIDTH*IMG_HEIGHT*4
#define RES_BUFF_SIZE 4096

//*************************************************************************
static int dm_cmd_probe(struct platform_device *pdev);
static int dm_cmd_open(struct inode *i, struct file *f);
static int dm_cmd_close(struct inode *i, struct file *f);
static ssize_t dm_cmd_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t dm_cmd_write(struct file *f, const char __user *buf, size_t count,
                         loff_t *off);
static int __init dm_cmd_init(void);
static void __exit dm_cmd_exit(void);
static int dm_cmd_remove(struct platform_device *pdev);

static char chToUpper(char ch);
static unsigned long strToInt(const char* pStr, int len, int base);
//*********************GLOBAL VARIABLES*************************************
static struct file_operations dm_cmd_fops =
  {
    .owner = THIS_MODULE,
    .open = dm_cmd_open,
    .release = dm_cmd_close,
    .read = dm_cmd_read,
    .write = dm_cmd_write
  };
static struct of_device_id dm_cmd_of_match[] = {
  { .compatible = "xlnx,dm-cmd-1.0", },
  { /* end of list */ },
};
static struct platform_driver dm_cmd_driver = {
  .driver = {
    .name = DRIVER_NAME,
    .owner = THIS_MODULE,
    .of_match_table	= dm_cmd_of_match,
  },
  .probe		= dm_cmd_probe,
  .remove	= dm_cmd_remove,
};
void * img_mem;
dma_addr_t img_dma_handle;

void * res_mem;
dma_addr_t res_dma_handle;

struct dm_cmd_info {
  unsigned long mem_start;
  unsigned long mem_end;
  void __iomem *base_addr;
};

static struct dm_cmd_info *tp = NULL;

MODULE_DEVICE_TABLE(of, dm_cmd_of_match);


static struct cdev c_dev;
static dev_t first;
static struct class *cl;


//***************************************************
// PROBE AND REMOVE

static int dm_cmd_probe(struct platform_device *pdev)
{
  struct resource *r_mem;
  int rc = 0;
  int x = 0;
  int y = 0;
  printk("dma_alloc_coherent\n");

  // Moja proba
  img_mem = dma_alloc_coherent(NULL, IMG_BUFF_SIZE, &img_dma_handle, GFP_DMA | __GFP_NOFAIL);
  if(!img_mem){
    printk(KERN_ALERT "Could not allocate dma_alloc_coherent for img");
    /* return -ENOMEM; */
  }
  else{
    printk("dma_alloc_coherent success img\n");
    for(y=0; y<IMG_HEIGHT; y++)
      for(x=0; x<IMG_WIDTH; x++){
        *((int*)(img_mem)+x+y*IMG_WIDTH) = img[y][x];
      }
  }
  printk("Allocated img addr virt: %p", img_mem);
  printk("Allocated img addr phys: %d", img_dma_handle);

  res_mem = dma_alloc_coherent(NULL, RES_BUFF_SIZE, &res_dma_handle, GFP_DMA | __GFP_NOFAIL);
  if(!res_mem){
    printk(KERN_ALERT "Could not allocate dma_alloc_coherent for res");
    /* return -ENOMEM; */
  }
  else{
    printk("dma_alloc_coherent success res\n");
    for(x=0; x<RES_BUFF_SIZE; x++){
      *((int*)(res_mem)+x) = 0;
    }
  }
  printk("Allocated res addr virt: %p", res_mem);
  printk("Allocated res addr phys: %d", res_dma_handle);

  //

  r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (!r_mem) {
    printk(KERN_ALERT "invalid address\n");
    return -ENODEV;
  }
  tp = (struct dm_cmd_info *) kmalloc(sizeof(struct dm_cmd_info), GFP_KERNEL);
  if (!tp) {
    printk(KERN_ALERT "Cound not allocate dm_cmd device\n");
    return -ENOMEM;
  }

  tp->mem_start = r_mem->start;
  tp->mem_end = r_mem->end;


  if (!request_mem_region(tp->mem_start,tp->mem_end - tp->mem_start + 1, DRIVER_NAME))
  {
    printk(KERN_ALERT "Couldn't lock memory region at %p\n",(void *)tp->mem_start);
    rc = -EBUSY;
    goto error1;
  }
  else {
    printk(KERN_INFO "dm_cmd_init: Successfully allocated memory region for dm_cmd\n");
  }
  /*
   * Map Physical address to Virtual address
   */

  tp->base_addr = ioremap(tp->mem_start, tp->mem_end - tp->mem_start + 1);
  if (!tp->base_addr) {
    printk(KERN_ALERT "dm_cmd: Could not allocate iomem\n");
    rc = -EIO;
    goto error2;
  }

  printk("probing done");
 error2:
  release_mem_region(tp->mem_start, tp->mem_end - tp->mem_start + 1);
 error1:
  return rc;

}

static int dm_cmd_remove(struct platform_device *pdev)
{
    /*
   * Exit Device Module
   */
  iounmap(tp->base_addr);
  release_mem_region(tp->mem_start, tp->mem_end - tp->mem_start + 1);
  return 0;
}

//***************************************************
// IMPLEMENTATION OF FILE OPERATION FUNCTIONS

static int dm_cmd_open(struct inode *i, struct file *f)
{
  printk("opening done");
  return 0;
}
static int dm_cmd_close(struct inode *i, struct file *f)
{
    printk("closing done");
    return 0;
}
static ssize_t dm_cmd_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk("reding entered");
    return 0;
}
static ssize_t dm_cmd_write(struct file *f, const char __user *buf, size_t count,
                           loff_t *off)
{
  char buffer[count];
  unsigned int addr;
  int i = 0;
  printk("writing enetered");
  i = copy_from_user(buffer, buf, count);
  buffer[count - 1] = '\0';
  addr = strToInt(buffer, count, 10);

  iowrite32(img_dma_handle, tp->base_addr + DATAMOVERCMD_MM2S_ADDR_OFFSET);
  iowrite32(IMG_BUFF_SIZE, tp->base_addr + DATAMOVERCMD_MM2S_BTT_OFFSET);
  iowrite32(res_dma_handle, tp->base_addr + DATAMOVERCMD_S2MM_ADDR_OFFSET);
  iowrite32(RES_BUFF_SIZE, tp->base_addr + DATAMOVERCMD_S2MM_BTT_OFFSET);
  iowrite32(1, tp->base_addr + DATAMOVERCMD_START_CMD_OFFSET);
  return count;
}

//***************************************************
// HELPER FUNCTIONS (STRING TO INTEGER)

static char chToUpper(char ch)
{
  if((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
    {
      return ch;
    }
  else
    {
      return ch - ('a'-'A');
    }
}

static unsigned long strToInt(const char* pStr, int len, int base)
{
  //                      0,1,2,3,4,5,6,7,8,9,:,;,<,=,>,?,@,A ,B ,C ,D ,E ,F
  static const int v[] = {0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,14,15};
  int i   = 0;
  unsigned long val = 0;
  int dec = 1;
  int idx = 0;

  for(i = len; i > 0; i--)
    {
      idx = chToUpper(pStr[i-1]) - '0';

      if(idx > sizeof(v)/sizeof(int))
	{
	  printk("strToInt: illegal character %c\n", pStr[i-1]);
	  continue;
	}

      val += (v[idx]) * dec;
      dec *= base;
    }

  return val;
}

//***************************************************
// INIT AND EXIT FUNCTIONS OF DRIVER

static int __init dm_cmd_init(void)
{

  printk(KERN_INFO "dm_cmd_init: Initialize Module \"%s\"\n", DEVICE_NAME);

  if (alloc_chrdev_region(&first, 0, 1, "DM_cmd_region") < 0)
  {
    printk(KERN_ALERT "<1>Failed CHRDEV!.\n");
    return -1;
  }
  printk(KERN_INFO "Succ CHRDEV!.\n");

  if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
  {
    printk(KERN_ALERT "<1>Failed class create!.\n");
    goto fail_0;
  }
  printk(KERN_INFO "Succ class chardev1 create!.\n");
  if (device_create(cl, NULL, MKDEV(MAJOR(first),0), NULL, "dm_cmd") == NULL)
  {
    goto fail_1;
  }

  printk(KERN_INFO "Device created.\n");

  cdev_init(&c_dev, &dm_cmd_fops);
  if (cdev_add(&c_dev, first, 1) == -1)
  {
    goto fail_2;
  }

  printk(KERN_INFO "Device init.\n");

  return platform_driver_register(&dm_cmd_driver);

 fail_2:
  device_destroy(cl, MKDEV(MAJOR(first),0));
 fail_1:
  class_destroy(cl);
 fail_0:
  unregister_chrdev_region(first, 1);
  return -1;

}

static void __exit dm_cmd_exit(void)
{
  int x = 0;

  platform_driver_unregister(&dm_cmd_driver);
  cdev_del(&c_dev);
  device_destroy(cl, MKDEV(MAJOR(first),0));
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  printk(KERN_ALERT "dm_cmd exit.\n");

  for(x=0; x<30; x++){
    printk("RES: %d \n", *((int*)(res_mem)+x));
  }
  /* kfree(img_mem); */
  dma_free_coherent(NULL, IMG_BUFF_SIZE, img_mem, img_dma_handle);
  dma_free_coherent(NULL, RES_BUFF_SIZE, res_mem, res_dma_handle);
  printk(KERN_INFO "dm_cmd_exit: Exit Device Module \"%s\".\n", DEVICE_NAME);
}

module_init(dm_cmd_init);
module_exit(dm_cmd_exit);

MODULE_AUTHOR ("Risto Pejasinovic");
MODULE_DESCRIPTION("Driver for AXI DATAMOVER_CMD");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("custom:DATAMOVERCMD");
