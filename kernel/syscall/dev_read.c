#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <errno.h>

int read(uint32_t fd,char *buf,int count)
{
	struct file * file;
	struct m_inode * inode;

	
	return -EINVAL;
}
