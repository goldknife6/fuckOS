SUBDIRS := minix1fs ramfs rootfs ttyfs pipefs


CURDIR := $(notdir $(shell pwd))

objects := $(wildcard *.c)
objects += $(wildcard *.S)
objects := $(patsubst %.c,%.o,$(objects))
objects := $(patsubst %.S,%.o,$(objects))
objects := $(patsubst %,$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%,$(objects))

DEPS = $(patsubst %.c,$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.d,$(wildcard *.c)) 

FILE 		:= $(ROOTDIR)/fs/*.sh
FILE 		:= $(wildcard $(FILE))
FILE 		:= $(notdir $(FILE))
FILE 		:= $(patsubst %.sh,$(ROOTDIR)/$(OBJDIR)/fs/%.bin,$(FILE))


.PHONY: all
all: $(SUBDIRS) $(DEPS) $(objects)  $(FILE)
	
$(DEPS):$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.d:%.c
	@mkdir -p $(@D)
	$(V)$(CC) -MT $(patsubst %.d,%.o,$@) $(ROOTDIR)/$(CURDIR)/$<  -ffreestanding -M $(INC) > $@


-include $(DEPS)

$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.bin:%.sh
	@echo $(notdir $^)
	@cp $(notdir $^) $(ROOTDIR)/$(OBJDIR)/$(CURDIR)
	@cd $(ROOTDIR)/$(OBJDIR)/$(CURDIR) ; $(OBJCOPY)  -I binary -O  elf32-i386 -B i386 $(notdir $^) $@

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
		

	



	
