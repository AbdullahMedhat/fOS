CC := i686-elf-gcc
NASM := nasm
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
NASMFLAGS := -felf32
LDFLAGS := -nostdlib -lgcc

SRCDIR := src
DEPDIR := deps
OBJDIR := obj
BINDIR := bin

CSRCFILES := $(shell find $(SRCDIR) -type f -name "*.c")
ASMSRCFILES := $(shell find $(SRCDIR) -type f -name "*.asm")

DEPFILES := $(CSRCFILES:$(SRCDIR)/%.c=$(DEPDIR)/%.d)

OBJFILES := $(CSRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o) $(ASMSRCFILES:$(SRCDIR)/%.asm=$(OBJDIR)/%.o)  

KERNELBIN := $(BINDIR)/fOS.bin
KERNELISO := $(BINDIR)/fOS.iso
LINKSCRIPT := linker.ld
GRUBCFG := grub.cfg

.PHONY: all clean

all: $(KERNELBIN)
	@echo "CSRCFILES : $(CSRCFILES)"
	@echo "ASMSRCFILES : $(ASMSRCFILES)"
	@echo "DEPFILES : $(DEPFILES)"
	@echo "OBJFILES : $(OBJFILES)"

$(KERNELBIN) : $(OBJFILES)
#	$(CC) -T $(LINKSCRIPT) $(LDFLAGS) -o $(KERNELBIN) $(OBJFILES)

-include $(DEPFILES)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d
	@echo "Compilando C: $@ - Dependencias: $^"
#	@$(CC) $(CFLAGS) -MMD -MP -MF -c $^ -o $@

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@echo "Generando dependencias: $@"
#	@$(CC) $(CFLAGS) -MM -MT '$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $<)' $< -MF $@

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@echo "Compilando ASM: $@ - Dependencias: $^"
#	@$(CC) $(CFLAGS) -MMD -MP -MF -c $^ -o $@
