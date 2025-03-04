
default: all


CSRC := $(wildcard src/driver/*.c)
CPPSRC := src/main.cpp $(wildcard src/driver/*.cpp)
INCDIR := src src/driver
# DDEFS := -DCH_USE_MUTEXES=1 -DLWIP_THREAD_STACK_SIZE=2048 -DPORT_INT_REQUIRED_STACK=128 -DCHIBIOS3 -DBASE_BOARD_H=$(BASE_BOARD_H) -DBOARD_CLASS=$(BOARD_CLASS) -DELEC_VERSION_$(REV)
# UDEFS :=

# Set default Makefile variables; these can be overridden by the project
include chibios/defaults.mk

# Add information specific to ChibiOS
include chibios/chibios.mk
CSRC += $(CHIBI_CSRC)
CPPSRC += $(CHIBI_CPPSRC)
INCDIR += $(CHIBI_INC)