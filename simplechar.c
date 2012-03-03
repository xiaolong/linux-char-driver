/*=============================================================
    A simple example of char device drivers
 ============================================================*/
#include <linux/module.h>
#include <linux/types.h> /* size_t */
#include <linux/fs.h>  /* everything */
#include <linux/errno.h> 
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h>/* copy_from/to_user() */

#include <linux/slab.h> /* kmalloc()*/

#define SIMPLECHAR_SIZE  0x1000  /* size of virtual device memory 4KB */
#define MEM_CLEAR 0x1  /*  ioctl operation code */
//#define SIMPLECHAR_MAJOR 254    /*  static device major */
#define SIMPLECHAR_MAJOR 0 /* dynamic device major */

static int simplechar_major = SIMPLECHAR_MAJOR;

/*simplechar : virtual char device struct */
struct simplechar_dev
{
  struct cdev cdev; /*cdev */
  unsigned char mem[SIMPLECHAR_SIZE]; /*virtual device memory size*/
  struct semaphore sem;//for concurrency control
};

struct simplechar_dev *simplechar_devp; 

int simplechar_open(struct inode *inode, struct file *filp)
{
  /* assign the virtual device struct to file private data */
  filp->private_data = simplechar_devp;
  return 0;
}
/* release funtion */
int simplechar_release(struct inode *inode, struct file *filp)
{
  return 0;
}

/* ioct operation function */
//static int simplechar_ioctl(struct inode *inodep, struct file *filp, unsigned int cmd, unsigned long arg)
static int simplechar_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
  struct simplechar_dev *dev = filp->private_data;

  switch (cmd)
  {
    case MEM_CLEAR:
      if(down_interruptible(&dev->sem)){//check semaphore
	return - ERESTARTSYS;
      }

      memset(dev->mem, 0, SIMPLECHAR_SIZE);
      up(&dev->sem);//release semaphore

      printk(KERN_INFO "globalmem is set to zero\n");
      break;

    default:
      return  - EINVAL;
  }
  return 0;
}

/* read device */
static ssize_t simplechar_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
  //printk(KERN_ALERT "READ %d \n", size);
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  struct simplechar_dev *dev = filp->private_data; 

  
  if (p >= SIMPLECHAR_SIZE)
    return count ?  - ENXIO: 0;
  if (count > SIMPLECHAR_SIZE - p)
    count = SIMPLECHAR_SIZE - p;

  if(down_interruptible(&dev->sem)){
    return - ERESTARTSYS;
    printk(KERN_ALERT "reading confilict! \n");
  }
  
  if (copy_to_user(buf, (void*)(dev->mem + p), count))
  {
    ret =  - EFAULT;
  }
  else
  {
    *ppos += count;
    ret = count;

    printk(KERN_INFO "read %u bytes(s) from %lu\n", count, p);
  }

  up(&dev->sem);

  return ret;
}

/* write device */
static ssize_t simplechar_write(struct file *filp, const char __user *buf,
  size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  struct simplechar_dev *dev = filp->private_data;

  
  if (p >= SIMPLECHAR_SIZE)
    return count ?  - ENXIO: 0;
  if (count > SIMPLECHAR_SIZE - p)
    count = SIMPLECHAR_SIZE - p;

  if(down_interruptible(&dev->sem)){
    return - ERESTARTSYS;
    printk(KERN_ALERT "writing confilict! \n");
  }
  //
  if (copy_from_user(dev->mem + p, buf, count))
    ret =  - EFAULT;
  else
  {
    *ppos += count;
    ret = count;

    printk(KERN_INFO "written %u bytes(s) from %lu\n", count, p);
  }
  
  up(&dev->sem);
  return ret;
}


static loff_t simplechar_llseek(struct file *filp, loff_t offset, int orig)
{
  loff_t ret = 0;
  switch (orig)
  {
    case 0:  //starting location
      if (offset < 0)
      {
        ret =  - EINVAL;
        break;
      }
      if ((unsigned int)offset > SIMPLECHAR_SIZE)
      {
        ret =  - EINVAL;
        break;
      }
      filp->f_pos = (unsigned int)offset;
      ret = filp->f_pos;
      break;
    case 1:   /* current location */
      if ((filp->f_pos + offset) > SIMPLECHAR_SIZE)
      {
        ret =  - EINVAL;
        break;
      }
      if ((filp->f_pos + offset) < 0)
      {
        ret =  - EINVAL;
        break;
      }
      filp->f_pos += offset;
      ret = filp->f_pos;
      break;
    default:
      ret =  - EINVAL;
      break;
  }
  return ret;
}

/* file_operations*/
static const struct file_operations simplechar_fops =
{
  .owner = THIS_MODULE,
  .llseek = simplechar_llseek,
  .read = simplechar_read,
  .write = simplechar_write,
  //.ioctl = simplechar_ioctl,//for kernel versions before 2.6, use this
  .unlocked_ioctl=simplechar_ioctl,
  .open = simplechar_open,
  .release = simplechar_release,
};

/* initialize and register cdev */
static void simplechar_setup_cdev(struct simplechar_dev *dev, int index)
{
  int err, devno = MKDEV(simplechar_major, index);

  cdev_init(&dev->cdev, &simplechar_fops);
  dev->cdev.owner = THIS_MODULE;
  //dev->cdev.ops = &simplechar_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if (err)
    printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

/*module initialization function*/
int simplechar_init(void)
{
  int result;
  dev_t devno = MKDEV(simplechar_major, 0);

  /* staticly allocate major number*/
  if (simplechar_major)
    result = register_chrdev_region(devno, 1, "simplechar");
  else  /*dynamically allocate major number*/
  {
    result = alloc_chrdev_region(&devno, 0, 1, "simplechar");
    simplechar_major = MAJOR(devno);
  }
  if (result < 0)
    return result;

  /* dynamically allocate memory for device struct*/
  simplechar_devp = (struct simplechar_dev*)kmalloc(sizeof(struct simplechar_dev), GFP_KERNEL);
  if (!simplechar_devp)    /* if fail*/
  {
    result =  - ENOMEM;
    goto fail_malloc;
  }
  memset(simplechar_devp, 0, sizeof(struct simplechar_dev));

  simplechar_setup_cdev(simplechar_devp, 0);//call the setup method
  //init_MUTEX(&simplechar_devp->sem);//not working on current kernel version, use sema_init() instead
  sema_init(&simplechar_devp->sem,1);

  printk(KERN_INFO "Init simplechar success!\n");
  return 0;

  fail_malloc: unregister_chrdev_region(devno, 1);
  return result;
}

/* module exit function */
void simplechar_exit(void)
{
  cdev_del(&simplechar_devp->cdev);   /*unregister cdev*/
  kfree(simplechar_devp);     /*release memory for device struct*/
  unregister_chrdev_region(MKDEV(simplechar_major, 0), 1); /*release major number*/
  printk(KERN_INFO "Bye simplechar!\n");
}

MODULE_AUTHOR("codehiker");
MODULE_LICENSE("Dual BSD/GPL");

module_param(simplechar_major, int, S_IRUGO);

module_init(simplechar_init);
module_exit(simplechar_exit);


