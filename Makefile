export CC 	:= gcc
export LD 	:= ld
export AR 	:= ar
export OBJCOPY 	:= objcopy
export OBJDUMP	:= objdump
export ROOTDIR 	:= $(shell pwd)
export OBJDIR 	:= obj

SUBDIRS 	:= fs kernel lib user 
LDFLAGS 	:= -m elf_i386 
KHANDER		:= -I $(ROOTDIR)/include/
UHANDER 	:= -I $(ROOTDIR)/include/
export INC 	:=  $(KHANDER)
CFLAGE		:=  -O1 -fno-builtin -fno-stack-protector  -fno-omit-frame-pointer -nostdlib
CFLAGE		+= -Wall -Wno-format -Wno-unused -Werror -gstabs -m32  -fno-tree-ch  
export UCFLAGS 	:= $(UHANDER) $(CFLAGE) -DUSER
export KCFLAGS 	:= $(KHANDER) $(CFLAGE) -DKERNEL  -DCONFIG_PAE -DCONFIG_DEBUG
export KLDFLAGS := $(LDFLAGS) -T kernel.ld 
export ULDFLAGS := -T user.ld $(LDFLAGS)
export V	:= @

export GCC_LIB 	:= $(shell $(CC) $(CFLAGE) -print-libgcc-file-name)

IMAGES		:= kenrel.iso
BOCHS		:= bochs
QEMU		:= qemu-system-i386 
QEMUOPTS	:= -m 60M -smp 4 -hdb fs/image.img 
GRUB		:= grub-mkrescue
IOSDIR		:= iso

#
OBJFILE 	:= kernel/init kernel/mm kernel/tty kernel/syscall\
		kernel/trap kernel/block fs/minix1fs fs/rootfs fs/ramfs fs/ttyfs fs kernel
OBJFILE 	:= $(patsubst %,%/*.S,$(OBJFILE)) $(patsubst %,%/*.c,$(OBJFILE))
OBJFILE 	:= $(wildcard $(OBJFILE))
OBJFILE 	:= $(patsubst %,$(ROOTDIR)/$(OBJDIR)/%,$(OBJFILE))
OBJFILE 	:= $(patsubst %.S,%.c,$(OBJFILE))
OBJFILE 	:= $(patsubst %.c,%.o,$(OBJFILE)) 

USEROBJ 	:= $(ROOTDIR)/user/*.c
USEROBJ 	:= $(wildcard $(USEROBJ))
USEROBJ 	:= $(patsubst %.c,%,$(USEROBJ))
USEROBJ 	:= $(notdir $(USEROBJ))
USEROBJ 	:= $(patsubst %,$(OBJDIR)/user/%.bin,$(USEROBJ))

OBJFILE 	+= obj/lib/string.o obj/lib/printfmt.o

# Eliminate default suffix rules
.SUFFIXES:
# Delete target files if there is an error (or make is interrupted)
.DELETE_ON_ERROR:


all:$(SUBDIRS)  $(ROOTDIR)/kern $(IMAGES)

$(ROOTDIR)/kern:$(OBJFILE) $(USEROBJ)
	@echo + $(LD)  -o $@ $(OBJFILE) $(USEROBJ)
	$(V)$(LD)  -o $@ $(KLDFLAGS) $(OBJFILE) $(USEROBJ)  $(GCC_LIB)
	$(V)cp kern $(IOSDIR)/boot/
	$(V)$(GRUB) -o $(IMAGES) $(IOSDIR)
	$(V)$(OBJDUMP) -S kern > asm.S

$(SUBDIRS):E
	$(V)make -s -C  $@
E:

$(IMAGES):$(ROOTDIR)/kern $(IOSDIR)/boot/grub/grub.cfg
	$(V)cp kern $(IOSDIR)/boot/
	$(V)$(GRUB) -o $@ $(IOSDIR) 

boch:all
	$(V)$(BOCHS)  -q -f bochsrc

qemu:all
	$(V)$(QEMU) $(QEMUOPTS) $(IMAGES)

.PHONY:clean
clean:
	$(V)rm -rf $(OBJDIR) kern
