#
# Copyright (c) 2012-2018 Andes Technology Corporation
# All rights reserved.
#
PROG 	?= Touch

# Select platform configure
# PLAT: AG101P, AE210P, AE100 AE3XX
# ADDR: 4GB, 16MB
# BUILD: HW, SIM
PLAT 	?= AE210P
ADDR 	?= 4GB
BUILD 	?= HW

# Debug and verbosity configuration
DEBUG	?= 1
V 		?= 0

# Cross compiler
CROSS_COMPILE ?= nds32le-elf-

# GCC compiler and bin utils
CC		:= $(CROSS_COMPILE)gcc
OBJDUMP	:= $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
AR		:= $(CROSS_COMPILE)ar
AS		:= $(CROSS_COMPILE)as

### Verbosity control. Use 'make V=1' to get verbose builds.
ifeq ($(V),1)
	TRACE_CC  =
	TRACE_C++ =
	TRACE_LD  =
	TRACE_AR  =
	TRACE_AS  =
	Q=
else
	TRACE_CC  = @echo "  CC       " $<
	TRACE_C++ = @echo "  C++      " $<
	TRACE_LD  = @echo "  LD       " $@
	TRACE_AR  = @echo "  AR       " $@
	TRACE_AS  = @echo "  AS       " $<
	Q=@
endif

# Debug or Release
ifeq ($(DEBUG),1)
	OPTIM   	:= -O0 -g3
	BUILD_DIR	= ./Debug
else
	OPTIM   	:= -Os -g3
	BUILD_DIR	= ./Release
endif

$(shell mkdir -p $(BUILD_DIR)) 

# Check parameters
ifneq ($(filter $(PLAT), AG101P AE210P AE100 AE3XX),$(PLAT))
	$(error Unknown PLAT "$(PLAT)" is not supported!)
endif

ifneq ($(filter $(ADDR), 4GB 16MB),$(ADDR))
	$(error Unknown ADDR "$(ADDR)" is not supported!)
endif

ifneq ($(filter $(BUILD), HW SIM), $(BUILD))
	$(error Unknown build mode "$(BUILD)" is not supported!)
endif

# Platform directory name
PLATFORM = $(shell echo $(PLAT) | tr A-Z a-z)

# Define the directories
CORE_DIR		= src

# The linker script
LDSCRIPT = $(CORE_DIR)/nds-amsi/bsp/$(PLATFORM)/nds32-$(PLATFORM).ld

# The header paths
INCS = \
	-I$(CORE_DIR) \
	-I$(CORE_DIR)/nds-adp-gpio \
	-I$(CORE_DIR)/nds-amsi \
	-I$(CORE_DIR)/nds-amsi/bsp \
	-I$(CORE_DIR)/nds-amsi/bsp/$(PLATFORM) \
	-I$(CORE_DIR)/nds-amsi/driver \
	-I$(CORE_DIR)/nds-amsi/driver/include \
	-I$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM) \
	-I$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/config \
	-I$(CORE_DIR)/uart \
	-I$(CORE_DIR)/uart/tinysh \
	-I$(CORE_DIR)/uart/nds-serial \
	-I$(CORE_DIR)/uart/gui-driver \

# The source files
SRCS = \
	$(CORE_DIR)/nds-amsi/bsp/start.S \
	$(CORE_DIR)/nds-amsi/bsp/startup-nds32.c \
	$(CORE_DIR)/nds-amsi/bsp/cache.c \
	$(CORE_DIR)/nds-amsi/bsp/$(PLATFORM)/irq.S \
	$(CORE_DIR)/nds-amsi/bsp/$(PLATFORM)/timer.c \
	$(CORE_DIR)/nds-amsi/bsp/$(PLATFORM)/$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/dma_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/gpio_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/i2c_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/pwm_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/rtc_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/spi_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/usart_$(PLATFORM).c \
	$(CORE_DIR)/nds-amsi/driver/v3/$(PLATFORM)/wdt_$(PLATFORM).c \
	$(CORE_DIR)/nds-adp-gpio/adp_gpio.c \
	$(CORE_DIR)/uart/terminal.c \
	$(CORE_DIR)/uart/tinysh/tinysh.c \
	$(CORE_DIR)/uart/nds-serial/serial.c \
	$(CORE_DIR)/uart/nds-serial/retarget.c \
	$(CORE_DIR)/uart/gui-driver/model.c \
	$(CORE_DIR)/uart/gui-driver/gui_cmd.c \
	$(CORE_DIR)/uart/gui-driver/gui_evt.c \
	$(CORE_DIR)/uart/gui-driver/cmd_validator.c \
	$(CORE_DIR)/main.c \

# Define all object files.
OBJX = $(patsubst %.S,%.o,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,${SRCS})))
OBJS = $(OBJX:$(CORE_DIR)/%.o=$(BUILD_DIR)/%.o)

# Compilation options
CMODEL = -mcmodel=large

CFLAGS = \
	-std=c11 \
	-Wall -Wextra -fstack-usage $(OPTIM) $(INCS) $(DEFINES) \
	-fomit-frame-pointer -fno-strict-aliasing -fno-builtin -funroll-loops \
	-ffunction-sections -fdata-sections \
	-mext-dsp \
	-include $(PLATFORM).h \
	$(CMODEL)

ASFLAGS = -D__ASSEMBLY__ $(CFLAGS) -c

LDFLAGS = \
	-T$(LDSCRIPT) $(OPTIM) \
	-static -nostartfiles \
	-Wl,-Map,$(BUILD_DIR)/$(PROG).map,--cref,--gc-sections,--undefine=__rtos_signature_freertos_v10_1_1 \
	$(CMODEL)

GCCVER 	= $(shell $(GCC) --version | grep gcc | cut -d" " -f9)

# Add `-fno-delete-null-pointer-checks` flag if the compiler supports it.
# GCC assumes that programs cannot safely dereference null pointers,
# and that no code or data element resides there.
# However, 0x0 is the vector table memory location, so the test must not be removed.
ifeq ($(shell $(CC) -fno-delete-null-pointer-checks -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
	CFLAGS  += -fno-delete-null-pointer-checks
	LDFLAGS += -fno-delete-null-pointer-checks
endif

### Make variables
DEFINES = -DCFG_$(ADDR)

ifneq ($(shell echo | $(CC) -E -dM - | grep __NDS32_ISA_V3M__ > /dev/null && echo V3M),V3M)
	ifeq ($(USE_CACHEWB),1)
		DEFINES += -DCFG_CACHE_ENABLE -DCFG_CACHE_WRITEBACK
	endif

	ifeq ($(USE_CACHEWT),1)
		DEFINES += -DCFG_CACHE_ENABLE -DCFG_CACHE_WRITETHROUGH
	endif
endif

# Compilation rules
.SUFFIXES : %.o %.c %.cpp %.S

$(BUILD_DIR)/%.o: $(CORE_DIR)/%.c
	$(TRACE_CC)
	@mkdir -p $(@D)
	$(Q)$(CC) -c -MMD $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(CORE_DIR)/%.S
	$(TRACE_CC)
	@mkdir -p $(@D)
	$(Q)$(CC) -c -MMD $(CFLAGS) -o $@ $<

$(BUILD_DIR)/$(PROG).elf : $(OBJS) 
	$(TRACE_LD)
	$(Q)$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	$(OBJDUMP) -S $@ > $(BUILD_DIR)/$(PROG).asm 
	$(OBJCOPY) -O binary -S $@ $(BUILD_DIR)/$(PROG).bin 

all: $(BUILD_DIR)/$(PROG).elf
	@echo '-----------------------------------------------------------------------'
	$(CROSS_COMPILE)size --format=berkeley $<
	@echo '-----------------------------------------------------------------------'
	@echo Completed

clean :
	@rm -rf $(BUILD_DIR)/*

connect:
	@cd $$(dirname $$(which ICEman)); ./ICEman --device 0

flash:
	@SPI_burn --unlock --lock --image $(BUILD_DIR)/$(PROG).bin 

# Automatic dependency generation
ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif

# http://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all clean connect flash