#include <fuckOS/fs.h>

#include <mm/slab.h>

#include <sys/stat.h>

struct list_head super_list = LIST_HEAD_INIT(super_list);


static struct super_block *read_super(int );
static void free_super(struct super_block *);
static int print_sector(struct buffer_head *);
static int print_super(struct super_block *);
static int print_inode(struct m_inode *);

struct super_block *
get_super(int dev)
{
	struct super_block *pos;

	if (!dev)
		return NULL;

	list_for_each_entry(pos, &super_list,  s_list) {
		if (pos->s_dev == dev)
			return pos;
	}

	return NULL;
}

void put_super(int dev)
{
	
	return;
}

static struct super_block *
alloc_super(int dev)
{
	if (!dev)
		return NULL;

	struct super_block *s;
	s = kmalloc(sizeof(struct super_block));
	if (!s)	
		return NULL;

	s->s_dev = dev;
	INIT_LIST_HEAD(&s->s_list);
	INIT_LIST_HEAD(&s->s_inodes);
	return s;
}

static void
free_super(struct super_block * s)
{
	
}

static struct super_block * 
read_super(int dev)
{
	struct super_block * s;
	struct buffer_head * bh;
	int i,block;

	if (!dev)
		return NULL;
	if ((s = get_super(dev)))
		return s;

	s = alloc_super(dev);
	if (!s)
		return NULL;

	if (!(bh = buffer_read(dev,1))) {
		s->s_dev=0;
		free_super(s);
		return NULL;
	}

	s->s_dev = dev;
	s->s_isup = NULL;
	s->s_imount = NULL;
	s->s_time = 0;
	s->s_rd_only = 0;
	s->s_dirt = 0;

	*((struct d_super_block *) s) = *((struct d_super_block *) bh->b_data);

	if (s->s_magic != SUPER_MAGIC) {
		s->s_dev = 0;
		free_super(s);
		return NULL;
	}
	

	for (i=0;i<I_MAP_SLOTS;i++)
		s->s_imap[i] = NULL;
	for (i=0;i<Z_MAP_SLOTS;i++)
		s->s_zmap[i] = NULL;

	block = 2;

	for (i = 0 ; i < s->s_imap_blocks ; i++) {
		if ((s->s_imap[i] = buffer_read(dev,block)))
			block++;
		else
			break;
	}

	for (i = 0 ; i < s->s_zmap_blocks ; i++) {
		if ((s->s_zmap[i] = buffer_read(dev,block)))
			block++;
		else
			break;
	}

	s->s_imap[0]->b_data[0] |= 1;
	s->s_zmap[0]->b_data[0] |= 1;

	list_add(&s->s_list,&super_list);

	
	//print_sector(s->s_zmap[0]);
	return s;
}
static void test()
{
	int i;
	struct m_inode *inode;
	struct buffer_head * buf;
	for (i =2; i < 3; i++) {
		inode = inode_get(0x307,i);
		assert(inode);
		print_inode(inode);
	}

	i = bmap(inode,0);

	
	buf = buffer_read(inode->i_dev,i);
	assert(buf);
//
	//print_sector(buf);
	//printk("%s\n",buf->b_data);
}

struct m_inode * 
mount_root()
{
	struct super_block *s;
	struct m_inode *rooti;
	s = read_super(ROOT_DEV);

	assert(s);
	rooti = inode_get(ROOT_DEV,ROOT_INODE);
	assert(rooti);
	//test();
	//print_super(s);
	print_inode(inode);
	return rooti;
}

static int print_sector(struct buffer_head *bh)
{
	assert(bh);
	int i = 0;
	int *buf = (int*)bh->b_data;
	for (i = 0;i < 512/4 ; i+=8) {
		printk("%08x %08x %08x %08x %08x %08x %08x %08x\n",
		buf[i],buf[i+1],buf[i+2],buf[i+3],buf[i+4],buf[i+5],buf[i+6],buf[i+7]);	
	}
	return 0;
}

static int print_super(struct super_block * s)
{
	assert(s);

	printk("s_ninodes:%d s_nzones:%d\n",s->s_ninodes,s->s_nzones);
	printk("s_imap_blocks:%d s_zmap_blocks:%d\n",s->s_imap_blocks,s->s_zmap_blocks);
	printk("s_firstdatazone:%d s_log_zone_size:%d\n",s->s_firstdatazone,s->s_log_zone_size);
	printk("s_max_size:%d s_magic:%x\n",s->s_max_size,s->s_magic);
	return 0;
}

static int print_inode(struct m_inode *i)
{
	assert(i);
	struct buffer_head * buf;
	printk("i_size:%d i_mtim:%d i_mode:0x%x dir:%d ",i->i_size,i->i_mtime,i->i_mode,S_ISDIR(i->i_mode));
	if (S_ISDIR(i->i_mode)) {
		int b;
		b = bmap(i,0);
		buf = buffer_read(i->i_dev,b);
		struct dir_entry *dir = (struct dir_entry *)buf->b_data;	
		printk("%s \n",dir->name);
	}
	printk("i_zone[0]:%d i_zone[1]:%x ",i->i_zone[0],i->i_zone[1]);
	printk("i_zone[2]:%d i_zone[3]:%x\n",i->i_zone[2],i->i_zone[3]);
	return 0;
}
