#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif

include	$(PSL1GHT)/ppu_rules

BUILD		:=	build
SOURCES		:=	.
INCLUDES	:=	.

#---------------------------------------------------------------------------------
ifeq ($(strip $(PLATFORM)),)
#---------------------------------------------------------------------------------
export BASEDIR		:= $(CURDIR)
export DEPS			:= $(BASEDIR)/deps
export LIBS			:= $(BASEDIR)/lib

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
export CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export CPPFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
export sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
export SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))

export OFILES		:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
						$(sFILES:.s=.o) $(SFILES:.S=.o)

export VPATH		:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

export INCLUDE		:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
						-I$(BASEDIR)/../../include

#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------

export LIBDIR		:= $(LIBS)/$(PLATFORM)
export DEPSDIR		:= $(DEPS)/$(PLATFORM)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------


LIBRARY		:= $(LIBDIR)/liblv2

#---------------------------------------------------------------------------------
LD			:=	$(PREFIX)ld -lfs_stub -lnet_stub -lrtc_stub -lio_stub -lc_stub -lssl_stub
LD		 	+=  -lcrashdump_system_export_stub \
				-lsysPrxForUser_export_stub \
				-lvsh_export_stub \
				-lpaf_export_stub \
				-lvshmain_export_stub \
				-lvshtask_export_stub \
				-lallocator_export_stub \
				-lsdk_export_stub \
				-lstdc_export_stub \
				-lpngdec_ppuonly_export_stub \
				-lxsetting_export_stub \
				-lsys_io_export_stub

CFLAGS		:= -O2 -mregnames -Wall -mcpu=cell $(MACHDEP) $(INCLUDE) -Wa,-mcell
ASFLAGS		:= $(MACHDEP) -mregnames -mcpu=cell -D__ASSEMBLY__ -Wa,-mcell $(INCLUDE)

#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------

all: ppu

#---------------------------------------------------------------------------------
ppu:
#---------------------------------------------------------------------------------
	@[ -d $(LIBS)/ppu ] || mkdir -p $(LIBS)/ppu
	@[ -d $(DEPS)/ppu ] || mkdir -p $(DEPS)/ppu
	@[ -d ppu ] || mkdir -p ppu
	@$(MAKE) PLATFORM=ppu lib -C ppu -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
install: all
#---------------------------------------------------------------------------------
	@[ -d $(PSL1GHT)/ppu/lib ] || mkdir -p $(PSL1GHT)/ppu/lib
	@cp -frv $(CURDIR)/lib/ppu/*.a $(PSL1GHT)/ppu/lib

#---------------------------------------------------------------------------------
$(LIBRARY).a: sprx.o
#---------------------------------------------------------------------------------

.PHONY: lib ppu install

#---------------------------------------------------------------------------------
lib: $(LIBRARY).a
#---------------------------------------------------------------------------------

libexport.o: libexport.c
	@$(CC) $(DEPSOPT) -S -m32 $(INCLUDE) $< -o libexport.S
	@$(CC) $(DEPSOPT) -c libexport.S -o $@

sprx.o: exports.o libexport.o
		@$(LD) -r exports.o libexport.o -o $@
#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	@echo clean ...
	@rm -rf ppu
	@rm -rf $(DEPS)
	@rm -rf $(LIBS)

-include $(DEPSDIR)/*.d