##############################################################################
# Project, target, sources and paths
#

# Define project name here
PROJECT = battery

# Target settings.
MCU  = cortex-m4

# Imported source files and paths.
CHIBIOS  := ./chibios/ChibiOS
CONFDIR  := ./chibios/config
# BOARDDIR := ./chibios/boards
BUILDDIR := ./build
DEPDIR   := ./.dep

# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32l4xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32L4xx/platform_l432.mk
# include $(CHIBIOS)/os/hal/boards/ST_NUCLEO32_L432KC/board.mk
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk
# Auto-build files in ./source recursively.
include $(CHIBIOS)/tools/mk/autobuild.mk
# Other files (optional).
# include $(CHIBIOS)/os/test/test.mk
# include $(CHIBIOS)/test/rt/rt_test.mk
# include $(CHIBIOS)/test/oslib/oslib_test.mk

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32L432xC.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CHIBI_CSRC = 	$(ALLCSRC) \
		$(TESTSRC) \
		$(CHIBIOS)/os/various/evtimer.c \
		$(wildcard ./chibios/bindings/*.c) \
		$(CHIBIOS)/os/various/syscalls.c 

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CHIBI_CPPSRC = $(ALLCPPSRC) \
		$(CHIBIOS)/os/various/cpp_wrappers/ch.cpp \
		$(wildcard ./chibios/bindings/*.cpp)

# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
CHIBI_INCDIR = $(CONFDIR) $(ALLINC) $(TESTINC) \
		 	$(CHIBIOS)/os/various/cpp_wrappers \
		 	./chibios/bindings \
			src src/hardware src/hardware/drivers src/modules


#
# Project, target, sources and paths
##############################################################################

