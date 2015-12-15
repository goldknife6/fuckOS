#include <sys/multiboot2.h>

#include <fuckOS/kernel.h>


extern multiboot_memory_map_t mmapinfo[128];
extern int nr_mmapinfo;

void detect_memory(void *_GrubMultiInfo)
{
	struct multiboot_tag *tag= (struct multiboot_tag *)(_GrubMultiInfo + 8);
	for (tag; tag->type != MULTIBOOT_TAG_TYPE_END;)
	{
		if(tag->type == MULTIBOOT_TAG_TYPE_MMAP)
		{
				multiboot_memory_map_t *mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				for (mmap ;(char*) mmap < (char *) tag + tag->size;) {
					mmapinfo[nr_mmapinfo].addr = mmap->addr;
					mmapinfo[nr_mmapinfo].len = mmap->len;
					mmapinfo[nr_mmapinfo].type = mmap->type;
					nr_mmapinfo++;
					mmap = (multiboot_memory_map_t *)((char *) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size);
				}
		}
		tag = (struct multiboot_tag *) ((char *) tag + ((tag->size + 7) & ~7));
	}
}
