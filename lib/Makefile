CURDIR := $(notdir $(shell pwd))

objects := $(wildcard *.c)
objects += $(wildcard *.S)
objects := $(patsubst %.c,%.o,$(objects))
objects := $(patsubst %.S,%.o,$(objects))

objects := $(patsubst %,$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%,$(objects))


libobj := $(wildcard *.c)
libobj += $(wildcard *.S)
libobj := $(patsubst %.c,$(ROOTDIR)/$(OBJDIR)/lib/%.o,$(libobj))
libobj := $(patsubst %.S,$(ROOTDIR)/$(OBJDIR)/lib/%.o,$(libobj))

.PHONY: all
all:$(objects) $(ROOTDIR)/$(OBJDIR)/$(CURDIR)/libOS.a
	
$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o:%.c
	@echo + cc $(CURDIR)/$<
	@mkdir -p $(@D)
	@$(CC)  -nostdinc $(UCFLAGS)  -c -o  $@ $< 
	
$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o:%.S
	@echo + cc $(CURDIR)/$<  
	@mkdir -p $(@D)
	@$(CC)  -nostdinc $(UCFLAGS)  -c  -o $@ $< 
	
$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/libOS.a:$(libobj)
	@$(AR) rv $@ $(libobj)
	
