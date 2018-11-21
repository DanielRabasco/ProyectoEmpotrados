#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/errno.h>
MODULE_LICENSE("Dual BSD/GPL");

static int minor = 0;
static int n_llamadas_apertura = 0;
static dev_t dev;
static struct cdev cdev;
static struct class *clase;
module_param(minor, int, S_IRUGO);

struct info_mydev {
	struct cdev mydev_cdev;
};

static int device_open(struct inode *inode, struct file *filp) {
	//struct info_mydev *info_dev = container_of(inode->i_cdev, struct info_mydev, mydev_cdev);
	//filp->private_data = info_dev;
	printk("FMODE: %i\n", filp->f_mode);
	printk("WRITE: %i\n", FMODE_WRITE);
	printk("READ: %i\n", FMODE_READ);
	printk("Funcion de apertura");
	return 0;
}

static int device_release(struct inode *inode, struct file *filp) {
	printk("Funcion de liberacion");
	return 0;
}

static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	printk("Funcion de escritura");
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
	printk("MAJOR:%i\n",MAJOR(dev));
	printk("MINOR:%i\n",MINOR(dev));
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