#include <drivers/sata.h>
#include <fuckOS/assert.h>

#include <mm/pages.h>


extern viraddr_t mmio_map_region(physaddr_t, size_t);
static void printreg(struct pci_func *pcif);

int pci_sata_attach(struct pci_func *pcif)
{

	pci_func_enable(pcif);

	// Memory map I/O for PCI device
	
	mmio_map_region(pcif->reg_base[5], pcif->reg_size[5]);
	


	return 1;
}

static void printreg(struct pci_func *pcif)
{
	int i = 0;
	for (i;i < 6;i++) {
		printk("reg_base:%x reg_size:%x\n",pcif->reg_base[i],pcif->reg_size[i]);
	}	
}
