//#include <asm/io.h>
//#include <asm/msr.h>
//#include <asm/errno.h>
//#include <linux/fs.h>
//#include <linux/pci.h>
//#include <linux/cdev.h>
//#include <linux/init.h>
//#include <linux/poll.h>
//#include <linux/delay.h>
//#include <linux/device.h>
//#include <linux/sched.h>
#include <linux/module.h>
//#include <linux/string.h>
#include <linux/kernel.h>
//#include <linux/i2c.h>
//#include <linux/mutex.h>
//#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>


#define PROCFS_USI_DIR	"usi"
#define PROCFS_USI_VERSION_DIR	"usi/version"
#define PROCFS_USI_VERSION_DIR_BIOS "bios"
#define PROCFS_MAX_SIZE	20

static struct proc_dir_entry *procfs_usi_dir;
static struct proc_dir_entry *procfs_usi_version_dir;
static struct proc_dir_entry *procfs_usi_version_bios_entry;
static char procfs_buf[PROCFS_MAX_SIZE];
static unsigned int procfs_buffer_size = 0;

static int write_profs(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	if(count > PROCFS_MAX_SIZE)
		procfs_buffer_size = PROCFS_MAX_SIZE;
	else
		procfs_buffer_size = count;
	if(copy_from_user(procfs_buf, buf, count))
		return -EFAULT;

	return procfs_buffer_size;
}

static int read_procfs(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
 	static int finished = 0;
	
	if(finished){
		finished = 0;
		return 0;
	}
		
	finished = 1;
		
	if(copy_to_user(buf, procfs_buf, procfs_buffer_size))
		return -EFAULT;
	
	return procfs_buffer_size;
}
static int open_procfs(struct inode *inode, struct file *filp){
	return 0;
}

static const struct file_operations bios_fops = {
	.owner = THIS_MODULE,
	.open = open_procfs,
	.read = read_procfs,
	.write = write_profs,
};

static int __init procfs_init(void)
{
	procfs_usi_dir = proc_mkdir(PROCFS_USI_DIR, NULL);
	procfs_usi_version_dir = proc_mkdir(PROCFS_USI_VERSION_DIR, NULL);
	procfs_usi_version_bios_entry = proc_create(PROCFS_USI_VERSION_DIR_BIOS, 0444, procfs_usi_version_dir, &bios_fops);
	
	return 0;
}

static void __exit procfs_exit(void)
{
	remove_proc_entry(PROCFS_USI_VERSION_DIR_BIOS, procfs_usi_version_dir);
	remove_proc_entry(PROCFS_USI_VERSION_DIR, NULL);
	remove_proc_entry(PROCFS_USI_DIR, NULL);
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");
