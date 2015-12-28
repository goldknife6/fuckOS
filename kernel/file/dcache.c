#include <fuckOS/dcache.h>
#include <fuckOS/list.h>

static struct hlist_head dentry_hashtable[MAX_DENTRY_HASH];


