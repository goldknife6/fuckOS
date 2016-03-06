#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <fuckOS/fs.h>
#include <fuckOS/ramfs.h>

#include <mm/pages.h>
#include <mm/slab.h>
#include <errno.h>
#include <string.h>
#define UTEMP2USTACK(addr)	((char*) (addr) + (USER_STACKTOP - PAGE_SIZE) - USER_TEMPBOTT)


int exit(struct task_struct *task);
int execve(char *filename,char **argv)
{
	int retval,i,argc,string_size;
	int *argv_store;
	char *string_store;
	struct nameidata nd;
	struct inode *inode;
	struct ram_file *ram;
	struct task_struct *task;
	struct frame *frame;
	retval = path_lookup(filename,0,LOOKUP_OPEN|LOOKUP_FOLLOW,&nd);

	if (retval < 0)
		return retval;

	inode = nd.dentry->d_inode;
	ram = (struct ram_file *)inode->i_inode;

	task = task_create(ram->start, 0,curtask->pid);

	if (!task)
		return -ENOMEM;

	*task->files = *curtask->files;
	*task->fs = *curtask->fs;	

	
	for (i = 0;i < MAX_FILE; i++) {
		if (curtask->files->fd[i]) {
			curtask->files->fd[i]->f_count++;
		}
	}

	frame = &task->frame;

	struct page *new = page_alloc(_GFP_ZERO);
	assert(new);

	page_insert(task->task_pgd, new,USER_STACKTOP - PAGE_SIZE,
				 _PAGE_PRESENT | _PAGE_RW | _PAGE_USER);

	page_insert(curtask->task_pgd, new,USER_TEMPBOTT,_PAGE_PRESENT | _PAGE_RW);

	string_size = 0;
	
	for (argc = 0; argv[argc] != 0; argc++)
		string_size += strlen(argv[argc]) + 1;

	string_store = (char*) USER_TEMPBOTT + PAGE_SIZE - string_size;

	argv_store = (int*) (ROUNDDOWN(string_store, 4) - 4 * (argc + 1));


	for (i = 0; i < argc; i++) {
		argv_store[i] = (int)UTEMP2USTACK(string_store);
		strcpy(string_store, argv[i]);
		string_store += strlen(argv[i]) + 1;
	}

	argv_store[argc] = 0;
	argv_store[-1] = (int)UTEMP2USTACK(argv_store);
	argv_store[-2] = argc;
	frame->tf_esp = (int)UTEMP2USTACK(&argv_store[-2]);
	page_remove(curtask->task_pgd, USER_TEMPBOTT) ;
	return task->pid;
}



