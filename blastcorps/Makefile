VERSION  := us.v10

BUILD_DIR = build

TOOLS_DIR := ../tools
ASM_PROCESSOR_DIR := $(TOOLS_DIR)/asm-processor

SRC_DIR = src.$(VERSION)

# timestamps
ifeq ($(VERSION),us.v10)
HD_CODE_TEXT_TIMESTAMP			= 855502464
HD_CODE_DATA_TIMESTAMP 			= 855502465
HD_FRONT_END_TEXT_TIMESTAMP = 855502466
HD_FRONT_END_DATA_TIMESTAMP = 855502466
else
# US v1.1
HD_CODE_TEXT_TIMESTAMP 			= 855919890
HD_CODE_DATA_TIMESTAMP 			= 855919891
HD_FRONT_END_TEXT_TIMESTAMP = 855919891
HD_FRONT_END_DATA_TIMESTAMP = 855919891
endif

# init
INIT_ASM_DIRS  = asm/init
INIT_BIN_DIRS  = assets/init
INIT_SRC_DIR   = $(SRC_DIR)/init
INIT_SRC_DIRS  = $(INIT_SRC_DIR)

INIT_S_FILES   = $(foreach dir,$(INIT_ASM_DIRS),$(wildcard $(dir)/*.s))
INIT_C_FILES   = $(foreach dir,$(INIT_SRC_DIRS),$(wildcard $(dir)/*.c))
INIT_BIN_FILES = $(foreach dir,$(INIT_BIN_DIRS),$(wildcard $(dir)/*.bin))

INIT_O_FILES := $(foreach file,$(INIT_S_FILES),$(BUILD_DIR)/$(file).o) \
                $(foreach file,$(INIT_C_FILES),$(BUILD_DIR)/$(file).o) \
                $(foreach file,$(INIT_BIN_FILES),$(BUILD_DIR)/$(file).o)

#asm/hd_code asm/hd_front_end
HD_CODE_ASM_DIRS  = asm/hd_code
HD_CODE_BIN_DIRS  = assets/hd_code
HD_CODE_SRC_DIR   = $(SRC_DIR)/hd_code
HD_CODE_SRC_DIRS  = $(HD_CODE_SRC_DIR)

HD_CODE_S_FILES   = $(foreach dir,$(HD_CODE_ASM_DIRS),$(wildcard $(dir)/*.s))
HD_CODE_C_FILES   = $(foreach dir,$(HD_CODE_SRC_DIRS),$(wildcard $(dir)/*.c))
HD_CODE_BIN_FILES = $(foreach dir,$(HD_CODE_BIN_DIRS),$(wildcard $(dir)/*.bin))

HD_CODE_O_FILES := $(foreach file,$(HD_CODE_S_FILES),$(BUILD_DIR)/$(file).o) \
                   $(foreach file,$(HD_CODE_C_FILES),$(BUILD_DIR)/$(file).o) \
                   $(foreach file,$(HD_CODE_BIN_FILES),$(BUILD_DIR)/$(file).o)


# Files requiring pre/post-processing
GREP    = grep -rl
GLOBAL_ASM_INIT_C_FILES := $(shell $(GREP) GLOBAL_ASM $(INIT_SRC_DIR) </dev/null 2>/dev/null)
GLOBAL_ASM_INIT_O_FILES := $(foreach file,$(GLOBAL_ASM_INIT_C_FILES),$(BUILD_DIR)/$(file).o)

GLOBAL_ASM_HD_CODE_C_FILES := $(shell $(GREP) GLOBAL_ASM $(HD_CODE_SRC_DIR) </dev/null 2>/dev/null)
GLOBAL_ASM_HD_CODE_O_FILES := $(foreach file,$(GLOBAL_ASM_HD_CODE_C_FILES),$(BUILD_DIR)/$(file).o)


# Tooling
CROSS   = mips-linux-gnu-
AS      = $(CROSS)as
CPP     = cpp
LD      = $(CROSS)ld
OBJDUMP = $(CROSS)objdump
OBJCOPY = $(CROSS)objcopy
PYTHON  = python3
GZIP    = gzip


CC      = $(TOOLS_DIR)/ido5.3_recomp/cc

OBJCOPYFLAGS = -O binary

OPT_FLAGS := -O1
MIPSISET := -mips2 -o32

INCLUDE_CFLAGS := -I . -I include -I include/2.0I -I include/2.0I/PR

ASFLAGS = -EB -mtune=vr4300 -march=vr4300 -mabi=32 -I include

CFLAGS := -G 0 -Xfullwarn -Xcpluscomm -signed -nostdinc -non_shared -Wab,-r4300_mul
CFLAGS += -D_LANGUAGE_C -D_FINALROM
# ignore compiler warnings about anonymous structs
CFLAGS += -woff 649,838
CFLAGS += $(INCLUDE_CFLAGS)

# Linker config
INIT_TARGET = $(BUILD_DIR)/init.$(VERSION)
INIT_LD_SCRIPT = init.$(VERSION).ld

HD_CODE_TARGET = $(BUILD_DIR)/hd_code.$(VERSION)
HD_CODE_LD_SCRIPT = hd_code.$(VERSION).ld

INIT_LDFLAGS    = -T $(INIT_LD_SCRIPT) -Map $(INIT_TARGET).map -T undefined_syms_auto.init.$(VERSION).txt -T undefined_funcs_auto.init.$(VERSION).txt -T undefined_syms.init.$(VERSION).txt --no-check-sections
HD_CODE_LDFLAGS = -T $(HD_CODE_LD_SCRIPT) -Map $(HD_CODE_TARGET).map -T undefined_syms_auto.hd_code.$(VERSION).txt -T undefined_funcs_auto.hd_code.$(VERSION).txt -T undefined_syms.hd_code.$(VERSION).txt --no-check-sections


### Optimisation Overrides


### Targets

default: all

all: verify_init verify_hd_code

dirs:
	$(foreach dir,$(INIT_SRC_DIR) $(INIT_ASM_DIRS) $(INIT_BIN_DIRS) $(HD_CODE_SRC_DIR) $(HD_CODE_ASM_DIRS) $(HD_CODE_BIN_DIRS),$(shell mkdir -p $(BUILD_DIR)/$(dir)))


init: dirs $(INIT_TARGET).bin

hd_code: dirs $(HD_CODE_TARGET).bin

verify_init: init
	@echo "$$(cat init.$(VERSION).sha1)  $(INIT_TARGET).bin" | sha1sum --check

verify_hd_code: hd_code
	@echo "$$(cat hd_code.$(VERSION).sha1)  $(HD_CODE_TARGET).bin" | sha1sum --check

extract:
	$(PYTHON) $(TOOLS_DIR)/splat/split.py init.$(VERSION).yaml
	$(PYTHON) $(TOOLS_DIR)/splat/split.py hd_code.$(VERSION).yaml

compress: $(BUILD_DIR)/hd_code_text.$(VERSION).gz $(BUILD_DIR)/hd_code_data.$(VERSION).gz
	@echo "Replacing extracted hd_code..."
	cp $(BUILD_DIR)/hd_code_text.$(VERSION).gz ../assets/hd_code_text.$(VERSION).bin
	cp $(BUILD_DIR)/hd_code_data.$(VERSION).gz ../assets/hd_code_data.$(VERSION).bin

clean:
	rm -rf asm
	rm -rf assets
	rm -rf build
	rm -f *auto.txt


### Recipes

$(BUILD_DIR)/init.$(VERSION).elf: $(INIT_O_FILES)
	@$(LD) $(INIT_LDFLAGS) -o $@

$(BUILD_DIR)/hd_code.$(VERSION).elf: $(HD_CODE_O_FILES)
	@$(LD) $(HD_CODE_LDFLAGS) -o $@

ifndef PERMUTER
$(GLOBAL_ASM_INIT_O_FILES): $(BUILD_DIR)/%.c.o: %.c
	$(PYTHON) $(ASM_PROCESSOR_DIR)/asm_processor.py $(OPT_FLAGS) $< > $(BUILD_DIR)/$<
	$(CC) -c -32 $(CFLAGS) $(OPT_FLAGS) $(MIPSISET) -o $@ $(BUILD_DIR)/$<
	$(PYTHON) $(ASM_PROCESSOR_DIR)/asm_processor.py $(OPT_FLAGS) $< --post-process $@ \
		--assembler "$(AS) $(ASFLAGS)" --asm-prelude $(ASM_PROCESSOR_DIR)/prelude.s

$(GLOBAL_ASM_O_HD_CODE_FILES): $(BUILD_DIR)/%.c.o: %.c
	$(PYTHON) $(ASM_PROCESSOR_DIR)/asm_processor.py $(OPT_FLAGS) $< > $(BUILD_DIR)/$<
	$(CC) -c -32 $(CFLAGS) $(OPT_FLAGS) $(MIPSISET) -o $@ $(BUILD_DIR)/$<
	$(PYTHON) $(ASM_PROCESSOR_DIR)/asm_processor.py $(OPT_FLAGS) $< --post-process $@ \
		--assembler "$(AS) $(ASFLAGS)" --asm-prelude $(ASM_PROCESSOR_DIR)/prelude.s
endif

# non asm-processor recipe
$(BUILD_DIR)/%.c.o: %.c
	$(CC) -c $(CFLAGS) $(OPT_FLAGS) $(MIPSISET) -o $@ $<

$(BUILD_DIR)/%.s.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%.bin.o: %.bin
	$(LD) -r -b binary -o $@ $<

%.bin: %.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

# compression

$(BUILD_DIR)/hd_code_text.$(VERSION).gz: $(BUILD_DIR)/hd_code_text.$(VERSION).bin
	$(PYTHON) $(TOOLS_DIR)/rarezip.py $< $@ --name hd_code_text.raw --time $(HD_CODE_TEXT_TIMESTAMP) --level 6

$(BUILD_DIR)/hd_code_data.$(VERSION).gz: $(BUILD_DIR)/hd_code_data.$(VERSION).bin
	$(PYTHON) $(TOOLS_DIR)/rarezip.py $< $@ --name hd_code_data.raw --time $(HD_CODE_DATA_TIMESTAMP) --level 6

$(BUILD_DIR)/hd_front_end_text.$(VERSION).gz: $(BUILD_DIR)/hd_front_end_text.$(VERSION).bin
	$(PYTHON) $(TOOLS_DIR)/rarezip.py $< $@ --name hd_front_end_text.raw --time $(HD_FRONT_END_TEXT_TIMESTAMP) --level 6

$(BUILD_DIR)/hd_front_end_data.$(VERSION).gz: $(BUILD_DIR)/hd_front_end_data.$(VERSION).bin
	$(PYTHON) $(TOOLS_DIR)/rarezip.py $< $@ --name hd_front_end_data.raw --time $(HD_FRONT_END_DATA_TIMESTAMP) --level 6

# split out sections
$(BUILD_DIR)/hd_code_text.$(VERSION).bin: $(BUILD_DIR)/hd_code.$(VERSION).elf
	$(OBJCOPY) -O binary --only-section .hd_code $< $@

$(BUILD_DIR)/hd_code_data.$(VERSION).bin: $(BUILD_DIR)/hd_code.$(VERSION).elf
	$(OBJCOPY) -O binary --only-section .hd_code_data $< $@

$(BUILD_DIR)/hd_front_end_text.$(VERSION).bin: $(BUILD_DIR)/hd_code.$(VERSION).elf
	$(OBJCOPY) -O binary --only-section .hd_front_end $< $@

$(BUILD_DIR)/hd_front_end_data.$(VERSION).bin: $(BUILD_DIR)/hd_code.$(VERSION).elf
	$(OBJCOPY) -O binary --only-section .hd_front_end_data $< $@


### Settings
.SECONDARY:
.PHONY: all clean default
SHELL = /bin/bash -e -o pipefail


#
# import datetime
# d = datetime.datetime.strptime("1997-02-14 11:31:31", '%Y-%m-%d %H:%M:%S')
# d.timestamp()
