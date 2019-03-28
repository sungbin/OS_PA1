#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/unistd.h>
#include <linux/cred.h>

MODULE_LICENSE("GPL");

void ** sctable ;
int count = 0;
int specified_Id = 0;
char logfile[10][128];

asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ; 

asmlinkage int m_open(const char __user * filename, int flags, umode_t mode)
{
	char fname[256] ;
	int input_user = current_uid().val;
	int i = 0;
	copy_from_user(fname, filename, 256) ;
	
	printk("input_user: %d\n",input_user);

	/* TODO: */
	if (specified_Id == input_user) {
		
	}

	return orig_sys_open(filename, flags, mode) ;
}


static 
int m_release(struct inode *inode, struct file *file) {
	return 0 ;
}


static
ssize_t m_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) 
{
	/* TODO: */
	char buf[256] ;
	ssize_t toread 
	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset ;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT ;	

	*offset = *offset + toread ;

	return toread ;
}

static 
ssize_t m_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) 
{

	return *offset ;
}

static const struct file_operations m_fops = {
	.owner = 	THIS_MODULE,
	.open = 	m_open,
	.read = 	m_read,
	.write = 	m_write,
	.llseek = 	seq_lseek,
	.release = 	m_release,
} ;

static 
int __init m_init(void) {
	unsigned int level;
	pte_t* pte;
	proc_create("dogdoor", S_IRUGO | S_IWUGO, NULL, &m_fops) ;

	sctable = (void *) kallsyms_lookup_name("sys_call_table") ;

	orig_sys_open = sctable[__NR_open];
	pte = lookup_address((unsigned long) sctable, &level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
	sctable[__NR_open] = m_sys_open;


	return 0;
}

static 
void __exit m_exit(void) {
	unsigned int level;
	pte_t* pte;
	remove_proc_entry("dogdoor", NULL) ;

	sctable[__NR_open] = orig_sys_open;
	pte = lookup_address((unsigned long) sctable, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

module_init(m_init);
module_exit(m_exit);
