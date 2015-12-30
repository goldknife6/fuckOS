SUBDIRS := minix1fs ramfs rootfs


CURDIR := $(notdir $(shell pwd))

objects := $(wildcard *.c)
objects += $(wildcard *.S)
objects := $(patsubst %.c,%.o,$(objects))
objects := $(patsubst %.S,%.o,$(objects))
objects := $(patsubst %,$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%,$(objects))

DEPS = $(patsubst %.c,$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.d,$(wildcard *.c)) 




.PHONY: all
all: $(SUBDIRS) $(DEPS) $(objects)
	
$(DEPS):$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.d:%.c
	@mkdir -p $(@D)
	$(V)$(CC) -MT $(patsubst %.d,%.o,$@) $(ROOTDIR)/$(CURDIR)/$<  -ffreestanding -M $(INC) > $@


-include $(DEPS)

$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o:%.c $(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.d
	@echo + cc $< 
	@mkdir -p $(@D)
	$(V)$(CC) $(KCFLAGS) -c $< -o $@
	
$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o:%.S
	@echo + cc $< 
	@mkdir -p $(@D)
	$(V)$(CC) $(KCFLAGS) -c $< -o $@




$(SUBDIRS):ECHO
	$(V)make -s -C $@
ECHO:
		

	



	
