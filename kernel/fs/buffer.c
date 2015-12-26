#include <fuckOS/fs.h>

#include <mm/slab.h>

#include <string.h>

#define HASH_SIZE	1024
#define _HASHFN(dev,block) (((unsigned)(dev^block))%HASH_SIZE)
#define HASH(dev,block) buffer_hash[_HASHFN(dev,block)]

static struct hlist_head *get_hash_table(int, uint32_t);
static struct buffer_head *buffer_get(int, uint32_t);
static struct buffer_head *buffer_alloc(int, uint32_t);
static void buffer_free(struct buffer_head *);
static void insert_into_hash(struct buffer_head *);
static void remove_from_hash(struct buffer_head *);


static struct hlist_head buffer_hash[HASH_SIZE];
static spinlock_t hash_lock;
static int buffer_count;

extern int block_read(struct buffer_head *);

static void print_info(struct buffer_head *bh)
{
	assert(bh);
	if (!bh->b_uptodate) return;
	int *buf = (int *)bh->b_data;
	int i;
	for (i = 0;i <512/4 ; i+=8) {
		printk("%08x %08x %08x %08x %08x %08x %08x %08x\n",
		buf[i],buf[i+1],buf[i+2],buf[i+3],buf[i+4],buf[i+5],buf[i+6],buf[i+7]);	
	}
	
}
void buffer_init()
{
	struct  hlist_head* begin = buffer_hash;
	struct  hlist_head* end = buffer_hash + HASH_SIZE;
	for (begin; begin < end; begin++) {
		INIT_HLIST_HEAD(begin);
	}
	SPIN_LOCK_INIT(&hash_lock);
	buffer_count = 0;

}

static struct buffer_head *
get_buffer_hash(int dev,uint32_t block)
{
	struct hlist_head *hlist = &HASH(dev,block);

	struct buffer_head *tmp = NULL;
	spin_lock(&hash_lock);

	hlist_for_each_entry(tmp, hlist, list_free){
		if (tmp->b_dev == dev && tmp->b_blocknr == block)
			goto unlock;
	}
unlock:
	spin_unlock(&hash_lock);
	return tmp;
}

static void insert_into_hash(struct buffer_head *buf)
{

	struct hlist_head *head = &HASH(buf->b_dev,buf->b_blocknr);
	spin_lock(&hash_lock);
	hlist_add_head(&buf->list_free, head);
	buffer_count++;
	spin_unlock(&hash_lock);
}


static void remove_from_hash(struct buffer_head *buf)
{
	struct buffer_head *node = NULL;
	struct hlist_head *head = &HASH(buf->b_dev,buf->b_blocknr);
	spin_lock(&hash_lock);
	hlist_for_each_entry(node, head, list_free){
		if (node->b_dev == buf->b_dev && node->b_blocknr == buf->b_blocknr) {
			hlist_del(&node->list_free);
			break;
		}
	}
	buffer_count--;
	spin_unlock(&hash_lock);
}


static struct buffer_head *
buffer_get(int dev,uint32_t block)
{
	struct buffer_head *tmp = NULL;

	tmp = get_buffer_hash(dev,block);

	if (tmp) return tmp;

	tmp = buffer_alloc(dev,block);
	
	if (!tmp)
		return NULL;

	insert_into_hash(tmp);

	return tmp;
}

static struct buffer_head *
buffer_alloc(int dev,uint32_t block)
{
	struct buffer_head *buf;
	buf = kmalloc(sizeof(struct buffer_head));

	if (!buf)
		return NULL;

	memset(buf,0x0,sizeof(struct buffer_head));
	buf->b_blocknr = block;
	buf->b_dev = dev;
	SPIN_LOCK_INIT(&buf->b_lock);
	INIT_HLIST_NODE(&buf->list_free);

	return buf;
}

static void buffer_free(struct buffer_head *buf)
{
	spin_lock(&buf->b_lock);
	if (--buf->b_count == 0) {
		if (buf->b_dirt) {
		}
		kfree(buf);
		buffer_count--;
	}
	spin_unlock(&buf->b_lock);
}

struct buffer_head *
buffer_read(int dev,uint32_t block)
{
	int retval;
	struct buffer_head *buf = NULL;
	buf = buffer_get(dev,block);
	if (!buf)
		return NULL;

	if (buf->b_uptodate)
		return buf;
	//ide read!!!
	retval = block_read(buf);

	if (buf->b_uptodate)
		return buf;
	return NULL;
}


int buffer_write(struct buffer_head *buf)
{
	return 0;
}
