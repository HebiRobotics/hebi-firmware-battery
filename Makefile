
BOARDDIR = ./boards
include $(BOARDDIR)/HEBI_PCBA_2121_01/board.mk


include chibios/defaults.mk
include chibios/chibios.mk

CSRC := 	$(wildcard src/*.c)
CPPSRC := 	$(wildcard ./src/*.cpp) \
			$(wildcard ./src/modules/*.cpp) \
			$(wildcard ./src/hardware/*.cpp) \
			$(wildcard ./src/hardware/drivers/*.cpp) \
			$(wildcard ./can-proto/driver/*.cpp) 

INCDIR := src src/hardware src/hardware/drivers src/modules can-proto

DDEFS := -DSTM32_EXTI_REQUIRED \
		 -DMFS_CFG_MEMORY_ALIGNMENT=8 
#-DCH_USE_MUTEXES=1 -DLWIP_THREAD_STACK_SIZE=2048 -DPORT_INT_REQUIRED_STACK=128 -DCHIBIOS3 -DBASE_BOARD_H=$(BASE_BOARD_H) -DBOARD_CLASS=$(BOARD_CLASS) -DELEC_VERSION_$(REV)
UDEFS :=

CSRC += $(CHIBI_CSRC)
CPPSRC += $(CHIBI_CPPSRC)
INCDIR += $(CHIBI_INCDIR)

ELECTRICAL_TYPE := BIB_B
BOARD_TYPE := WATTMAN


##############################################################################
# General Project Information
#

# Extract version and repository information from git
GIT_REVISION:= $(shell git log -1 | head -1 | cut -d ' ' -f 2 | cut -c-7 )
GIT_PATH := $(shell git rev-parse --show-prefix)
GIT_MODIFIED := $(shell ( ! git diff --cached -s --exit-code || ! git diff --exit-code -s) && echo "(modified)")
# Shorthand 'modified' tag
GIT_MOD := $(shell ( ! git diff --cached -s --exit-code || ! git diff --exit-code -s) && echo "-m")

# Auto-generate summary information for firmware.  If BUILD_LABEL
# is provided, use that; otherwise, use git revision
FIRMWARE_REVISION := $(if $(BUILD_LABEL),$(BUILD_LABEL),$(GIT_PATH)@$(GIT_REVISION)$(GIT_MOD))

FIRMWARE_TAG := $(DESCRIPTION) $(GIT_MODIFIED)
FIRMWARE_DATE := $(shell date)
FIRMWARE_USERNAME := $(shell whoami | sed 's,\\\\,/,')

# List all default C defines here, like -D_DEBUG=1
COMMON_DDEFS = -D_FIRMWARE_REVISION="$(FIRMWARE_REVISION)"
COMMON_DDEFS += -D_FIRMWARE_TAG="$(FIRMWARE_TAG)"
COMMON_DDEFS += -D_FIRMWARE_DATE="$(FIRMWARE_DATE)"
COMMON_DDEFS += -D_FIRMWARE_USERNAME="$(FIRMWARE_USERNAME)"
COMMON_DDEFS += -D_FIRMWARE_TYPE="$(FIRMWARE_TYPE)"
COMMON_DDEFS += -D_ELECTRICAL_TYPE="$(ELECTRICAL_TYPE)"
# For string manipulation:
COMMON_DDEFS += -D_FIRMWARE_MODE="$(FIRMWARE_MODE)"
# For preprocessor ifdefs:
COMMON_DDEFS += -D_FIRMWARE_MODE_$(FIRMWARE_MODE)

DDEFS += $(COMMON_DDEFS)

#
# General Project Information
##############################################################################


##############################################################################
# Common rules
#

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################