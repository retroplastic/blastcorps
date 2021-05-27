BASENAME  = blastcorps
VERSION  := us.v10

BUILD_DIR = build
ASM_DIRS  = asm
BIN_DIRS  = assets

TOOLS_DIR := tools

S_FILES   = $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.s))
BIN_FILES = $(foreach dir,$(BIN_DIRS),$(wildcard $(dir)/*.bin))

O_FILES := $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file).o) \
           $(foreach file,$(BIN_FILES),$(BUILD_DIR)/$(file).o)

BLASTCORP_EXTRACTED := blastcorps/init.$(VERSION).bin blastcorps/hd_code.$(VERSION).bin blastcorps/hd_front_end.$(VERSION).bin

TARGET = $(BUILD_DIR)/$(BASENAME).$(VERSION)
LD_SCRIPT = $(BASENAME).$(VERSION).ld

CROSS = mips-linux-gnu-
AS = $(CROSS)as
CPP = cpp
LD = $(CROSS)ld
OBJDUMP = $(CROSS)objdump
OBJCOPY = $(CROSS)objcopy
PYTHON = python3
GZIP = gzip

OBJCOPYFLAGS = -O binary

ASFLAGS = -EB -mtune=vr4300 -march=vr4300 -mabi=32 -I include

LDFLAGS = -T $(LD_SCRIPT) -Map $(TARGET).map --no-check-sections

### Optimisation Overrides

### Targets

default: all

all: dirs $(TARGET).z64 verify

dirs:
	$(foreach dir,$(SRC_DIRS) $(ASM_DIRS) $(BIN_DIRS),$(shell mkdir -p $(BUILD_DIR)/$(dir)))

check: .baserom.$(VERSION).ok

verify: $(TARGET).z64
	@echo "$$(cat $(BASENAME).$(VERSION).sha1)  $(TARGET).z64" | sha1sum --check

extract: check assets/init.$(VERSION).bin

clean:
	rm -rf asm
	rm -rf assets
	rm -rf build
	rm -f *auto.txt
	rm -rf $(BLASTCORP_EXTRACTED)

decompress: $(BLASTCORP_EXTRACTED)

### Recipes

# decompression
assets/%.$(VERSION).ext: assets/%.$(VERSION).bin
	$(GZIP) -d -S ".bin" $< -c > $@

blastcorps/hd_code.$(VERSION).bin: assets/hd_code_text.$(VERSION).ext assets/hd_code_data.$(VERSION).ext
	cat assets/hd_code_text.$(VERSION).ext assets/hd_code_data.$(VERSION).ext > $@

blastcorps/hd_front_end.$(VERSION).bin: assets/hd_front_end_text.$(VERSION).ext assets/hd_front_end_data.$(VERSION).ext
	cat assets/hd_front_end_text.$(VERSION).ext assets/hd_front_end_data.$(VERSION).ext > $@

blastcorps/init.$(VERSION).bin: assets/init.$(VERSION).bin
	cp assets/init.$(VERSION).bin $@

assets/init.$(VERSION).bin:
	$(PYTHON) $(TOOLS_DIR)/splat/split.py $(BASENAME).$(VERSION).yaml

.baserom.$(VERSION).ok: baserom.$(VERSION).z64
	@echo "$$(cat $(BASENAME).$(VERSION).sha1)  $<" | sha1sum --check
	@touch $@

$(TARGET).elf: $(O_FILES)
	@$(LD) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.s.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%.bin.o: %.bin
	$(LD) -r -b binary -o $@ $<

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(TARGET).z64: $(TARGET).bin
	@cp $< $@

### Settings
.SECONDARY:
.PHONY: all clean default
SHELL = /bin/bash -e -o pipefail
