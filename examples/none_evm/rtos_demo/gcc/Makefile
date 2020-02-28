PROJECT_NAME     := ble_basic_function
TARGETS          := fr8010h
OUTPUT_DIRECTORY := build

SDK_ROOT := ../../../..
PROJ_DIR := ../code

LINKER_SCRIPT  := ldscript.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/proj_main.c \
  $(PROJ_DIR)/prf_client.c \
  $(PROJ_DIR)/prf_server.c \
  $(PROJ_DIR)/user_task.c \
  $(PROJ_DIR)/app_at.c \
  $(PROJ_DIR)/rtos_entry.c \
  $(SDK_ROOT)/components/driver/driver_adc.c \
  $(SDK_ROOT)/components/driver/driver_codec.c \
  $(SDK_ROOT)/components/driver/driver_efuse.c \
  $(SDK_ROOT)/components/driver/driver_exti.c \
  $(SDK_ROOT)/components/driver/driver_i2s.c \
  $(SDK_ROOT)/components/driver/driver_iic.c \
  $(SDK_ROOT)/components/driver/driver_keyscan.c \
  $(SDK_ROOT)/components/driver/driver_pmu.c \
  $(SDK_ROOT)/components/driver/driver_pmu_pwm.c \
  $(SDK_ROOT)/components/driver/driver_pmu_qdec.c \
  $(SDK_ROOT)/components/driver/driver_pwm.c \
  $(SDK_ROOT)/components/driver/driver_rtc.c \
  $(SDK_ROOT)/components/driver/driver_ssp.c \
  $(SDK_ROOT)/components/driver/driver_system.c \
  $(SDK_ROOT)/components/driver/driver_timer.c \
  $(SDK_ROOT)/components/driver/driver_uart.c \
  $(SDK_ROOT)/components/driver/driver_wdt.c \
  $(SDK_ROOT)/components/modules/platform/source/app_boot_vectors.c \
  $(SDK_ROOT)/components/modules/platform/source/exception_handlers.c \
  $(SDK_ROOT)/components/modules/patch/patch.c \
  $(SDK_ROOT)/components/modules/button/button.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR) \
  $(SDK_ROOT)/components/ble/include \
  $(SDK_ROOT)/components/ble/include/gap \
  $(SDK_ROOT)/components/ble/include/gatt \
  $(SDK_ROOT)/components/ble/include/mesh \
  $(SDK_ROOT)/components/driver/include \
  $(SDK_ROOT)/components/modules/os/include \
  $(SDK_ROOT)/components/modules/sys/include \
  $(SDK_ROOT)/components/modules/platform/include \
  $(SDK_ROOT)/components/modules/common/include \
  $(SDK_ROOT)/components/modules/lowpow/include \
  $(SDK_ROOT)/components/modules/button \
  $(SDK_ROOT)/components/modules/FreeRTOS/include \
  $(SDK_ROOT)/components/modules/FreeRTOS/portable/gcc \

# Libraries common to all targets
LIB_FILES += -lfr8010h_stack

# Optimization flags
OPT = -Os -g

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -mcpu=cortex-m3
CFLAGS += -mthumb
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fmessage-length=0 -fsigned-char
CFLAGS += -std=gnu11

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m3
ASMFLAGS += -mthumb

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb
LDFLAGS += -mcpu=cortex-m3
LDFLAGS +=  -T$(LINKER_SCRIPT) -L$(SDK_ROOT)/components/ble/library
LDFLAGS += $(SDK_ROOT)/components/ble/library/syscall_gcc.txt
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))
