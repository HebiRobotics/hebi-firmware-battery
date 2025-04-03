
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


##############################################################################
# Common rules
#

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################