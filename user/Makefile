
CURDIR := $(notdir $(shell pwd))

objects := $(wildcard *.c)
objects += $(wildcard *.S)



objects := $(patsubst %.c,%.o,$(objects))
objects := $(patsubst %.S,%.o,$(objects))

 

objects := $(patsubst %,$(OBJDIR)/$(CURDIR)/%,$(objects))
objects := $(patsubst %,$(ROOTDIR)/%,$(objects))



BinObjects := $(wildcard *.c) $(wildcard *.S)
BinObjects := $(patsubst %.c,%,$(BinObjects))
BinObjects := $(patsubst %.S,%,$(BinObjects))
BinObjects := $(patsubst %,$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%,$(BinObjects))




.PHONY: all
all:$(objects) $(BinObjects)
	
$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o:%.c
	@echo + cc [user] $(CURDIR)/$< 
	@mkdir -p $(@D)
	$(CC) $(UCFLAGS) -nostdinc  -c -o $@ $< 
	

$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o:%.S
	@echo + cc [user] $(CURDIR)/$<
	@mkdir -p $(@D)
	$(CC)  $(UCFLAGS) -nostdinc  -c  -o $@ $<

$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%:$(ROOTDIR)/$(OBJDIR)/$(CURDIR)/%.o \
				    $(ROOTDIR)/$(OBJDIR)/lib/entry.o

	@echo + $(LD) [user] $(notdir $@) 
	@$(LD) -o $@  $(ULDFLAGS) $(LDFLAGS) $@.o -L$(ROOTDIR)/$(OBJDIR)/lib -lOS $(GCC_LIB)
	@cd $(ROOTDIR)/$(OBJDIR)/$(CURDIR) ; $(OBJCOPY)  -I binary -O  elf32-i386 -B i386 $(notdir $@) $@.bin
	@$(OBJDUMP) -S $@ > $@.asm



