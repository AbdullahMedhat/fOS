CC := i686-elf-gcc
NASM := nasm
MKDIR := mkdir
CP := cp
GRUB-MKRESCUE := grub-mkrescue

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
NASMFLAGS := -felf32
LDFLAGS := -nostdlib -lgcc

SRCDIR := src
DEPDIR := deps
OBJDIR := obj

CSRCFILES := $(shell find $(SRCDIR) -type f -name "*.c")
ASMSRCFILES := $(shell find $(SRCDIR) -type f -name "*.asm")

DEPFILES := $(CSRCFILES:$(SRCDIR)/%.c=$(DEPDIR)/%.d)

OBJFILES := $(CSRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o) $(ASMSRCFILES:$(SRCDIR)/%.asm=$(OBJDIR)/%.o)  

KERNELBIN := bin/fOS.bin
KERNELISO := iso/fOS.iso
LINKSCRIPT := config/linker.ld
GRUBCFG := config/grub.cfg

.PHONY: all kernel iso clean

all: $(KERNELBIN) iso

kernel : $(KERNELBIN)

iso : $(KERNELISO)	

ifeq (0, $(words $(findstring $(MAKECMDGOALS), clean)))

-include $(DEPFILES)

endif
	
$(KERNELBIN) : $(OBJFILES) $(LINKSCRIPT) Makefile
	@echo "Linkeando Kernel $@ - Dependencias: $^"
	@$(MKDIR) -p $(dir $@)
	@$(CC) -T $(LINKSCRIPT) $(LDFLAGS) -o $(KERNELBIN) $(OBJFILES)
	@echo ''

$(KERNELISO) : $(KERNELBIN) $(GRUBCFG) Makefile
	@echo "Generando ISO $@"
	@$(MKDIR) -p $(dir $@)isodir/boot/grub
	@$(CP) $< $(dir $@)isodir/boot/
	@$(CP) $(GRUBCFG) $(dir $@)isodir/boot/grub/grub.cfg
	@$(GRUB-MKRESCUE) -o $@ $(dir $@)isodir

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d Makefile
	@echo "Compilando C: $@ - Dependencias: $<"
	@$(MKDIR) -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $<

$(DEPDIR)/%.d: $(SRCDIR)/%.c Makefile
	@echo "Generando dependencias: $@"
	@$(MKDIR) -p $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT '$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $<)' $< -MF $@

$(OBJDIR)/%.o: $(SRCDIR)/%.asm Makefile
	@echo "Compilando ASM: $@ - Dependencias: $<"
	@$(MKDIR) -p $(dir $@)
	@$(NASM) $(NASMFLAGS) -o $@ $<

clean:
	$(RM) -f -r $(OBJDIR) $(DEPDIR) $(dir $(KERNELBIN)) $(dir $(KERNELISO))

