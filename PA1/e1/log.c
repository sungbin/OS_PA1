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
int specified_Id;
char logfile[10][128];

asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ; 

asmlinkage int log_sys_open(const char __user * filename, int flags, umode_t mode)
{
	char fname[256] ;
	int input_user = current_uid().val;
	int i = 0;
	copy_from_user(fname, filename, 256) ;

	if (specified_Id == input_user) {
		if(count != 10){
			strncpy(logfile[count], filename, 127);
			count++;
		}else{
			for(i = 0; i < 9; i++){
				strncpy(logfile[i], logfile[i+1], 127);
			}
			strncpy(logfile[count - 1], filename, 127);
		}

	}

	return orig_sys_open(filename, flags, mode) ;
}


static 
int log_proc_open(struct inode *inode, struct file *file) {
	return 0 ;
}

static 
int log_proc_release(struct inode *inode, struct file *file) {
	return 0 ;
}

static
ssize_t log_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset) 
{
	char buf[256] ;
	ssize_t toread ;
	int i = 0;

	for(i = 0; i < count; i++){
		sprintf(buf, "%d. %s\n",count + 1,  logfile[i]) ;
	}

	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset ;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT ;	

	*offset = *offset + toread ;

	return toread ;
}

static 
ssize_t log_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) 
{
	char buf[128] ;
	char m_temp[128] = {0x0,};
	if (*offset != 0 || size > 128)
		return -EFAULT ;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT ;

	sscanf(buf, "%128s", m_temp);

	if(m_temp[0]=='u'){
		int res = 0;
		int i = 0;
		for(i = 0; m_temp[i] != '\0'; i++){
			res = res * 10 + m_temp[i] - '0';
		}
		specified_Id = res;
	}

	count = 0 ;
	*offset = strlen(buf) ;

	return *offset ;
}

static const struct file_operations log_fops = {
	.owner = 	THIS_MODULE,
	.open = 	log_proc_open,
	.read = 	log_proc_read,
	.write = 	log_proc_write,
	.llseek = 	seq_lseek,
	.release = 	log_proc_release,
} ;

static 
int __init log_init(void) {
	unsigned int level ; 
	pte_t * pte ;

	proc_create("log", S_IRUGO | S_IWUGO, NULL, &log_fops) ;

	sctable = (void *) kallsyms_lookup_name("sys_call_table") ;

	orig_sys_open = sctable[__NR_open] ;
	pte = lookup_address((unsigned long) sctable, &level) ;
	if (pte->pte &~ _PAGE_RW) 
		pte->pte |= _PAGE_RW ;		
	sctable[__NR_open] = log_sys_open ;

	return 0;
}

static 
void __exit log_exit(void) {
	unsigned int level ;
	pte_t * pte ;
	remove_proc_entry("log", NULL) ;

	sctable[__NR_open] = orig_sys_open ;
	pte = lookup_address((unsigned long) sctable, &level) ;
	pte->pte = pte->pte &~ _PAGE_RW ;
}

module_init(log_init);
module_exit(log_exit);
