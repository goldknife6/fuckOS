
CURDIR := $(notdir $(shell pwd))

objects := $(wildcard *.c)
objects += $(wildcard *.S)
objects := $(patsubst %.c,%.o,$(objects))
objects := $(patsubst %.S,%.o,$(objects))
objects := $(patsubst %,$(ROOTDIR)/$(OBJDIR)/kernel/$(CURDIR)/%,$(objects))

DEPS = $(patsubst %.c,$(ROOTDIR)/$(OBJDIR)/kernel/$(CURDIR)/%.d,$(wildcard *.c)) 




.PHONY: all
all:$(DEPS) $(objects)
	
$(DEPS):$(ROOTDIR)/$(OBJDIR)/kernel/$(CURDIR)/%.d:%.c
	@mkdir -p $(@D)
	$(CC) -MT $(patsubst %.d,%.o,$@) $(ROOTDIR)/kernel/$(CURDIR)/$<  -ffreestanding -M $(INC) > $@


-include $(DEPS)

$(ROOTDIR)/$(OBJDIR)/kernel/$(CURDIR)/%.o:%.c $(ROOTDIR)/$(OBJDIR)/kernel/$(CURDIR)/%.d
	@echo $(CC) $(KCFLAGS) -c $< -o $@
	@mkdir -p $(@D)
	$(CC) $(KCFLAGS) -c $< -o $@
	
$(ROOTDIR)/$(OBJDIR)/kernel/$(CURDIR)/%.o:%.S
	@echo $(CC) $(KCFLAGS) -c $< -o $@
	@mkdir -p $(@D)
	$(CC) $(KCFLAGS) -c $< -o $@
	



