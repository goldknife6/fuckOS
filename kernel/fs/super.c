#include <fuckOS/fs.h>

struct super_block super_block[NR_SUPER];


struct super_block *get_super(int dev)
{
	struct super_block *s;

	if (!dev)
		return NULL;
	s = super_block;
	while (s < NR_SUPER+super_block)
		if (s->s_dev == dev) {
			if (s->s_dev == dev)
				return s;
			s = super_block;
		} else
			s++;
	return NULL;
}
