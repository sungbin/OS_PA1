#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static 
char m_name[128] = { 0x0, } ;
static
int m_num=0;

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
	char buf[256] ;
	ssize_t toread ;

//	sprintf(buf, "Hello %s from kernel!\n", m_name) ;
	sprintf(buf, "my num is %d\n",m_num);

	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset ;

	if (copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT ;	

	*offset = *offset + toread ;

	return toread ;
}

static 
ssize_t m_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset) 
{
	char buf[128] ;

	if (*offset != 0 || size > 128)
		return -EFAULT ;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT ;

	char m_temp[128] = { 0x0, } ;
	sscanf(buf,"%128s", m_temp) ;

	/*change type string as integer*/	
	int res=0;
	int i;

	for(i = 0; m_temp[i] != '\0'; ++i) {
		res = res*10 +m_temp[i] - '0';

	}
	m_num = res;
	/*  */

	*offset = strlen(buf) ;

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
	proc_create("mexe", S_IRUGO | S_IWUGO, NULL, &m_fops) ;
	return 0;
}

static 
void __exit m_exit(void) {
	remove_proc_entry("mexe", NULL) ;
}


module_init(m_init);
module_exit(m_exit);
