#include <fuckOS/fs.h>
#include <fuckOS/task.h>
#include <fuckOS/assert.h>

#include <sys/stat.h>

#include <string.h>
#include <errno.h>

static struct buffer_head * find_entry(struct m_inode ** dir,
	const char * name, int namelen, struct dir_entry ** res_dir)
{
	int entries;
	int block,i;
	struct buffer_head * bh;
	struct dir_entry * de;
	struct super_block * sb;

	entries = (*dir)->i_size / (sizeof (struct dir_entry));
	*res_dir = NULL;

	

	if (!namelen)
		return NULL;

	if (!(block = (*dir)->i_zone[0]))
		return NULL;

	if (!(bh = buffer_read((*dir)->i_dev,block)))
		return NULL;


	

	i = 0;
	de = (struct dir_entry *) bh->b_data;
	while (i < entries) {
		if ((uint8_t *)de >= BLOCK_SIZE + bh->b_data) {
			//brelse(bh);
			bh = NULL;
			if (!(block = bmap(*dir,i/DIR_ENTRIES_PER_BLOCK)) ||
			    !(bh = buffer_read((*dir)->i_dev,block))) {
				i += DIR_ENTRIES_PER_BLOCK;
				continue;
			}
			de = (struct dir_entry *) bh->b_data;
		}
		if (!strncmp(name,(const char *)de->name,namelen)) {
			*res_dir = de;
			return bh;
		}
		de++;
		i++;
	}
	//brelse(bh);
	return NULL;
}


static struct m_inode * 
get_dir(const char *pathname)
{
	char c;
	const char * thisname;
	struct m_inode * inode;
	struct buffer_head * bh;
	int namelen,inr,idev;
	struct dir_entry * de;

	inode = curtask->files->root;

	while (1) {
		thisname = pathname;

		if (!S_ISDIR(inode->i_mode)) {
			return NULL;
		}
		
		for (namelen=0; (c = *(pathname++) ) && (c!='/'); namelen++)
			/* nothing */ ;
		if (!c)
			return inode;
		
		if (!(bh = find_entry(&inode,thisname,namelen,&de))) {
			//iput(inode);
			return NULL;
		}
		inr = de->inode;
		idev = inode->i_dev;
	}
}

static struct m_inode * dir_namei(const char * pathname,
	int * namelen, const char ** name)
{
	char c;
	const char * basename;
	struct m_inode * dir;
	
	if (!(dir = get_dir(pathname)))
		return NULL;

	basename = pathname;
	while ( (c= *(pathname++)) )
		if (c=='/')
			basename=pathname;
	*namelen = pathname-basename-1;
	*name = basename;
	return dir;
}


int open_namei(const char * pathname, int flag, int mode,
	struct m_inode ** res_inode)
{
	const char * basename;
	int inr,dev,namelen;
	struct m_inode * dir, *inode;
	struct buffer_head * bh;
	struct dir_entry * de;

	
	if (!(dir = dir_namei(pathname,&namelen,&basename))) {
		return -ENOENT;
	}
	
	bh = find_entry(&dir,basename,namelen,&de);
	
	if (!bh) {
		return -ENOENT;
	}
	
	inode = inode_get(dir->i_dev,de->inode);
	*res_inode = inode;
	return 0;
}
