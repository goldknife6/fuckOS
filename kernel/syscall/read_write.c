#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <error.h>

int read(uint32_t fd,char * buf,int count)
{
	struct file * file;
	struct m_inode * inode;

	if (fd >= MAX_FILES || count < 0 || !(file = &curtask->files->fd[fd]))
		return -EINVAL;

	if (!count)
		return 0;

	//verify_area(buf,count);
	inode = file->f_inode;
/*
	if (inode->i_pipe)
		return (file->f_mode&1)?read_pipe(inode,buf,count):-EIO;
	if (S_ISCHR(inode->i_mode))
		return rw_char(READ,inode->i_zone[0],buf,count,&file->f_pos);
	if (S_ISBLK(inode->i_mode))
		return block_read(inode->i_zone[0],&file->f_pos,buf,count);
*/
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
