#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <errno.h>

int read(uint32_t fd,char * buf,int count)
{
	struct file * file;
	struct m_inode * inode;

	if (fd >= MAX_FILES || count < 0 || !(file = curtask->files->fd[fd])) {
		return -EINVAL;
	}
	if (!count)
		return 0;

	inode = file->f_inode;

	//if (S_ISDIR(inode->i_mode) || S_ISREG(inode->i_mode)) {
		if (count + file->f_pos > inode->i_size) {
			count = inode->i_size - file->f_pos;
		} if (count<=0) {
			return 0;
		}
		return file_read(inode,file,buf,count);
	//}
	printk("(Read)inode->i_mode=%06o\n\r",inode->i_mode);
	return -EINVAL;
}
