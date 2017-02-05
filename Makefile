.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
	$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/3ds_rules

CFGFILE ?= Makefile.config
include $(CURDIR)/$(CFGFILE)

TARGET   := $(BINDIR)/$(BINNAME)

LIBS     := $(foreach lib,$(LIBRARIES),-l$(lib))
LIBDIRS  := $(CTRULIB)

OUTPUT   := $(CURDIR)/$(TARGET)

VPATH    := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

DEPSDIR  := $(CURDIR)/$(BUILD)

CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))

OFILES   := $(addprefix $(BUILD)/, $(PICAFILES:.v.pica=.shbin.o) $(CPPFILES:.cpp=.o) $(CFILES:.c=.o))

LD       := $(CC)

INCLUDE  := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
            $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
            $(foreach dir,$(PORTLIBS),-I$(dir)/include) \
            -I$(CURDIR)/$(BUILD)

LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
            $(foreach dir,$(PORTLIBS),-L$(dir)/lib)

APP_ICON := $(CURDIR)/$(ICON)

_3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh --romfs=$(CURDIR)/$(ROMFS)

# Compiler flags

ARCH     := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS   := -g -Wall -O3 -Wextra -mword-relocations \
            -fomit-frame-pointer -ffunction-sections \
            $(ARCH) $(EXTRACFLAGS)

CFLAGS   += $(INCLUDE) -DARM11 -D_3DS

CXXFLAGS := $(CFLAGS) -fno-rtti -fexceptions -std=gnu++11

LDFLAGS   = -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

# Shortcuts

all : prereq $(OUTPUT).3dsx $(OUTPUT).cia
3dsx: prereq $(OUTPUT).3dsx
cia : prereq $(OUTPUT).cia

prereq:
	@[ -d $(CURDIR)/$(BUILD) ] || mkdir -p $(CURDIR)/$(BUILD)
	@[ -d $(CURDIR)/$(BINDIR) ] || mkdir -p $(CURDIR)/$(BINDIR)

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(BINDIR) $(TARGET).3dsx $(OUTPUT).cia $(OUTPUT).smdh $(TARGET).elf $(BINNAME).lst $(BINNAME).map

# Output

MAKEROM ?= makerom
3DSTOOL ?= 3dstool

$(OUTPUT).elf: $(OFILES)

$(OUTPUT).3dsx: $(OUTPUT).elf $(OUTPUT).smdh

$(OUTPUT).cia: $(OUTPUT).elf $(BUILD)/banner.bnr $(BUILD)/icon.icn
	$(3DSTOOL) -cvtf romfs $(BUILD)/romfs.bin --romfs-dir $(CURDIR)/$(ROMFS)
	$(MAKEROM) -f cia -o $@ -elf $< -rsf $(CURDIR)/resources/rominfo.rsf -target t -exefslogo -banner $(BUILD)/banner.bnr -icon $(BUILD)/icon.icn -romfs $(BUILD)/romfs.bin -DAPP_TITLE="$(APP_TITLE)" -DPRODUCT_CODE="$(PRODUCT_CODE)" -DUNIQUE_ID="$(UNIQUE_ID)"
	@echo "built ... $(BINNAME).cia"

# Banner

BANNERTOOL ?= bannertool

ifeq ($(suffix $(BANNER_IMAGE)),.cgfx)
	BANNER_IMAGE_ARG := -ci
else
	BANNER_IMAGE_ARG := -i
endif

ifeq ($(suffix $(BANNER_AUDIO)),.cwav)
	BANNER_AUDIO_ARG := -ca
else
	BANNER_AUDIO_ARG := -a
endif

$(BUILD)/%.bnr: $(BANNER_IMAGE) $(BANNER_AUDIO)
	$(BANNERTOOL) makebanner $(BANNER_IMAGE_ARG) $(BANNER_IMAGE) $(BANNER_AUDIO_ARG) $(BANNER_AUDIO) -o $@

$(BUILD)/%.icn: $(ICON)
	$(BANNERTOOL) makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -i $(ICON) -f visible,nosavebackups -o $@

# Source

DEPENDS := $(OFILES:.o=.d)

$(BUILD)/%.o: %.cpp
	@echo $(notdir $<)
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

$(BUILD)/%.o: %.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@ $(ERROR_FILTER)
	
#---------------------------------------------------------------------------------
# rules for assembling GPU shaders
#---------------------------------------------------------------------------------
define shader-as
	$(eval CURBIN := $(patsubst %.shbin.o,%.shbin,$(notdir $@)))
	picasso -o $(BUILD)/$(CURBIN) $1
	bin2s $(BUILD)/$(CURBIN) | $(AS) -o $@
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(BUILD)/$(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(BUILD)/$(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(BUILD)/$(CURBIN) | tr . _)`.h
endef

$(BUILD)/%.shbin.o : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

$(BUILD)/%.shbin.o : %.v.pica
	@echo $(notdir  $<)
	@$(call shader-as, $<)

-include $(DEPENDS)
