#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/stat.h>
#include <asm/uaccess.h>

#define VIRTUALDISK_SIZE    0x2000 //8K
#define MEM_CLEAR 0x1
#define PORT1_SET 0x2
#define PORT2_SET 0x3

#define VIRTUALDISK_MAJOR   200

static int virtual_disk_major = VIRTUALDISK_MAJOR;

/*struct of virtual disk*/
struct virtual_disk
{
    struct cdev cdev;  //cdev struct
    unsigned char mem[VIRTUALDISK_SIZE];
    int port1;
    long port2;
    long count; //count of open num
};

struct virtual_disk* virtual_dsik_devp = NULL;

static void virtual_disk_setup(struct virtual_disk *dev, int minor_devno);

static loff_t virtual_disk_llseek(struct file *file, loff_t offset, int org);

static ssize_t virtual_disk_read(struct file* file, char __user *buf, size_t size, loff_t *ppos);
static ssize_t virtual_disk_write(struct file* file, const char __user *buf, size_t size, loff_t *ppos);

static long virtual_disk_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int virtual_disk_open(struct inode* inode, struct file* file);
int virtual_disk_release(struct inode* inode, struct file* file);

static const struct file_operations virtual_disk_fops = 
{
    .owner = THIS_MODULE,
    .llseek = virtual_disk_llseek,
    .read = virtual_disk_read,
    .write = virtual_disk_write,
    .unlocked_ioctl = virtual_disk_ioctl,
    .open = virtual_disk_open,
    .release = virtual_disk_release,
};

int virtual_disk_init(void)
{
    int result = 0;
    dev_t devno = MKDEV(virtual_disk_major, 0);

    if(virtual_disk_major)
    {
        result = register_chrdev_region(devno, 1, "virtual_disk");
    }
    else
    {
        result = alloc_chrdev_region(&devno, 0, 1, "virtual_disk");

        virtual_disk_major = MAJOR(devno);//get major devno from devno allocated
    }

    if(result < 0)
        return result;

    virtual_dsik_devp = kmalloc(sizeof(struct virtual_disk), GFP_KERNEL);
    if(!virtual_dsik_devp)
    {
        result = -ENOMEM;
        goto fail_kmalloc;
    }

    memset(virtual_dsik_devp, 0, sizeof(struct virtual_disk));

    virtual_disk_setup(virtual_dsik_devp, 0);

    return 0;

fail_kmalloc:
    unregister_chrdev_region(devno, 1);
    return result;
}

void virtual_disk_exit(void)
{
    cdev_del(&virtual_dsik_devp->cdev);
    kfree(virtual_dsik_devp);
    unregister_chrdev_region(MKDEV(virtual_disk_major, 0), 1);
}

static void virtual_disk_setup(struct virtual_disk *dev, int minor_devno)
{
    int err = 0;

    int devno = MKDEV(virtual_disk_major, minor_devno);
    
    cdev_init(&dev->cdev, &virtual_disk_fops);//init dev devices

    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &virtual_disk_fops;

    err = cdev_add(&dev->cdev, devno, 1);//register char device into kernel

    if(err)
        printk("error in cdev_add()\n");
}

int virtual_disk_open(struct inode* inode, struct file* file)
{
    file->private_data = virtual_dsik_devp;
    struct virtual_disk* devp = file->private_data;

    devp->count++;

    return 0;
}

int virtual_disk_release(struct inode* inode, struct file* file)
{
    struct virtual_disk* devp = file->private_data;

    devp->count--;

    return 0;
}

static ssize_t virtual_disk_read(struct file* file, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned long count = size;
    int ret = 0;

    struct virtual_disk* devp = file->private_data;

    if(p >= VIRTUALDISK_SIZE)
        return count?-ENXIO:0;

    if(count > VIRTUALDISK_SIZE-p)
        count = VIRTUALDISK_SIZE - p;

    if(copy_to_user(buf, (void*)(devp->mem+p), count))
    {
        ret = -1;
    }
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "read %d bytes from %d\n", count, p);
    }

    return ret;
}

static ssize_t virtual_disk_write(struct file* file, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned long count = size;
    int ret = 0;

    struct virtual_disk* devp = file->private_data;

    if(p >= VIRTUALDISK_SIZE)
        return count?-ENXIO:0;

    if(count > VIRTUALDISK_SIZE-p)
        count = VIRTUALDISK_SIZE - p;

    if(copy_from_user(devp->mem+p, buf, count))
    {
        ret = -1;
    }
    else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "written %d bytes from %d\n", count, p);
    }

    return ret;
}

static loff_t virtual_disk_llseek(struct file *file, loff_t offset, int org)
{
    loff_t ret = 0;

    switch(org)
    {
        case SEEK_SET:
        {
            if(offset < 0)
            {
                ret = -EINVAL;
                break;
            }

            if((unsigned int)offset > VIRTUALDISK_SIZE)
            {
                ret = -EINVAL;
                break;
            }

            file->f_pos = (unsigned int)offset;
            ret = file->f_pos;
        }
        break;
        case SEEK_CUR:
        {
            if(((file->f_pos+offset)>VIRTUALDISK_SIZE) || ((file->f_pos+offset)<VIRTUALDISK_SIZE))
            {
                ret = -EINVAL;
                break;
            }
            
            file->f_pos += offset;
            ret = file->f_pos;
        }
        break;
        default :
        {
            ret = -EINVAL;    
        }
        break;
    }

    return ret;
}

static long virtual_disk_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct virtual_disk *devp = file->private_data;

    switch(cmd)
    {
        case MEM_CLEAR:
        {
            memset(devp->mem, 0, VIRTUALDISK_SIZE);
            printk(KERN_INFO "virtual disk is set to all zero\n");
        }
        break;
        case PORT1_SET:
        {
            devp->port1 = 0;
            printk(KERN_INFO "virtual disk port1 set to zero\n");
        }
        break;
        case PORT2_SET:
        {
            devp->port2 = 0;
            printk(KERN_INFO "virtual disk port2 set to zero\n");
        }
        break;
        default :
        {
            printk(KERN_INFO "err to ioctl cmd=%d\n", cmd);
            return -EINVAL;
        }
        break;
    }

    return 0;
}

module_init(virtual_disk_init);
module_exit(virtual_disk_exit);

MODULE_LICENSE("GPL");

