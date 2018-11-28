#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/errno.h>
MODULE_LICENSE("Dual BSD/GPL");

static int minor = 0;
static int n_aperturas_writemode = 0;
static dev_t dev;
static struct cdev cdev;
static struct class *clase;
module_param(minor, int, S_IRUGO);

struct info_mydev {
	struct cdev mydev_cdev;
};

static int device_open(struct inode *inode, struct file *filp) {
	struct info_mydev *info_dev = container_of(inode->i_cdev, struct info_mydev, mydev_cdev);
	filp->private_data = info_dev;

	//Check that there is only one call to open in write-mode
	if ((filp->f_mode & FMODE_WRITE) != 0){
		if (n_aperturas_writemode == 0){
			n_aperturas_writemode++;
			printk("WRITE");
		}else{
			return -EBUSY;
		}
	}
	printk(KERN_ALERT "Funcion de apertura");
	return 0;
}


static int device_release(struct inode *inode, struct file *filp) {
	//If the open call was in write mode, decrease the counter and allow other write-mode open call
	if ((filp->f_mode & FMODE_WRITE) != 0){
		n_aperturas_writemode--;
	}
	printk(KERN_ALERT "Funcion de liberacion");
	return 0;
}

static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	printk(KERN_ALERT "Funcion de escritura");
	return count;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = device_open,
  .release = device_release,
  .write = device_write
};

static int carga_init(void){
	alloc_chrdev_region(&dev, minor, 1, "spkr");
	cdev_init(&cdev, &fops);
	cdev_add(&cdev, dev, 1);
	clase = class_create(THIS_MODULE, "speaker");
	device_create(clase, NULL, dev, NULL, "intspkr");
	printk(KERN_ALERT "MAJOR:%i\n",MAJOR(dev));
	printk(KERN_ALERT "MINOR:%i\n",MINOR(dev));
	return 0;
}

static void descarga_exit(void){
	unregister_chrdev_region(dev, 1);
	cdev_del(&cdev);
	device_destroy(clase, dev);
	class_destroy(clase);
}

module_init(carga_init);
module_exit(descarga_exit);