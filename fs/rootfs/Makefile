
CURDIR := $(notdir $(shell pwd))

objects := $(wildcard *.c)
objects += $(wildcard *.S)
objects := $(patsubst %.c,%.o,$(objects))
objects := $(patsubst %.S,%.o,$(objects))
objects := $(patsubst %,$(ROOTDIR)/$(OBJDIR)/fs/$(CURDIR)/%,$(objects))

DEPS = $(patsubst %.c,$(ROOTDIR)/$(OBJDIR)/fs/$(CURDIR)/%.d,$(wildcard *.c)) 




.PHONY: all
all:$(DEPS) $(objects)
	
$(DEPS):$(ROOTDIR)/$(OBJDIR)/fs/$(CURDIR)/%.d:%.c
	@mkdir -p $(@D)
	$(CC) -MT $(patsubst %.d,%.o,$@) $(ROOTDIR)/fs/$(CURDIR)/$<  -ffreestanding -M $(INC) > $@


-include $(DEPS)

$(ROOTDIR)/$(OBJDIR)/fs/$(CURDIR)/%.o:%.c $(ROOTDIR)/$(OBJDIR)/fs/$(CURDIR)/%.d
	@echo $(CC) $(KCFLAGS) -c $< -o $@
	@mkdir -p $(@D)
	$(V)$(CC) $(KCFLAGS) -c $< -o $@
	
$(ROOTDIR)/$(OBJDIR)/fs/$(CURDIR)/%.o:%.S
	@echo $(CC) $(KCFLAGS) -c $< -o $@
	@mkdir -p $(@D)
	$(V)$(CC) $(KCFLAGS) -c $< -o $@
	



