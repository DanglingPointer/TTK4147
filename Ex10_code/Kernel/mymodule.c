#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>

static struct proc_dir_entry *proc_file_entry;

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	if (offset > 0)
	{
		return 0;
	}
	else
	{
		return sprintf(buffer, "Hello world\n");
	}
}

static int __init init_mymodule(void) {
	proc_file_entry = create_proc_entry("myproc",0644,NULL);
	proc_file_entry->read_proc = procfile_read;
	printk("Mymodle initing....\n\n");
	return 0;
}

static void __exit exit_mymodule(void) {
	remove_proc_entry("myproc", NULL);
	printk("\nExiting mymodule.....\n\n");
	return;	
}

module_init(init_mymodule);
module_exit(exit_mymodule);
