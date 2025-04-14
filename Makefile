# MicroPython port for the NEORV32 RISC-V Processor
# Copyright (c) 2025 Stephan Nolting

# Include the core environment definitions; this will set $(TOP)
include micropython/py/mkenv.mk

# Include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

NEORV32_SW_HOME = neorv32/sw

# Define the required assembly source files
SRC_S = $(NEORV32_SW_HOME)/common/crt0.S

# Define the required C source files
SRC_C = \
	$(wildcard *.c) \
  $(wildcard $(NEORV32_SW_HOME)/lib/source/*.c) \
	shared/readline/readline.c \
	shared/runtime/gchelper_generic.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c

# Compiler prefix and ISA configuration
CROSS_COMPILE ?= riscv-none-elf-
MARCH         ?= rv32i_zicsr_zifencei
MABI          ?= ilp32

# Compiler settings
CFLAGS += -I . -I $(TOP) -I $(BUILD) -I $(NEORV32_SW_HOME)/lib/include
CFLAGS += -march=$(MARCH) -mabi=$(MABI) -Os -Wall -ffunction-sections -fdata-sections -nostartfiles
CFLAGS += -mno-fdiv -mstrict-align -mbranch-cost=10 -Wl,--gc-sections -ffp-contract=off -g
CFLAGS += -T $(NEORV32_SW_HOME)/common/neorv32.ld

# Memory layout (base address and size)
CFLAGS += -Wl,--defsym,__neorv32_rom_base=0x00000000 -Wl,--defsym,__neorv32_rom_size=512k
CFLAGS += -Wl,--defsym,__neorv32_ram_base=0x01000000 -Wl,--defsym,__neorv32_ram_size=256k

# Define source files containing qstrs
SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c

# Define the required object files
OBJ  = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_S:.S=.o))

# Define the top-level target, the main firmware
all: $(BUILD)/firmware.elf

# Define how to build the firmware
$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^ -lm -lc -lgcc
	$(Q)$(SIZE) $@

# Include remaining core make rules
include $(TOP)/py/mkrules.mk
