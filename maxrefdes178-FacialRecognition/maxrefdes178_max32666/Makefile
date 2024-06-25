################################################################################
 # Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 #
 # Permission is hereby granted, free of charge, to any person obtaining a
 # copy of this software and associated documentation files (the "Software"),
 # to deal in the Software without restriction, including without limitation
 # the rights to use, copy, modify, merge, publish, distribute, sublicense,
 # and/or sell copies of the Software, and to permit persons to whom the
 # Software is furnished to do so, subject to the following conditions:
 #
 # The above copyright notice and this permission notice shall be included
 # in all copies or substantial portions of the Software.
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 # OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 # MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 # IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 # OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 # ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 # OTHER DEALINGS IN THE SOFTWARE.
 #
 # Except as contained in this notice, the name of Maxim Integrated
 # Products, Inc. shall not be used except as stated in the Maxim Integrated
 # Products, Inc. Branding Policy.
 #
 # The mere transfer of this software does not imply any licenses
 # of trade secrets, proprietary technology, copyrights, patents,
 # trademarks, maskwork rights, or any other form of intellectual
 # property whatsoever. Maxim Integrated Products, Inc. retains all
 # ownership rights.
 #
 #
 ###############################################################################

# This is the name of the build output file
ifeq "$(PROJECT)" ""
PROJECT=maxrefdes178_max32666
endif

# Specify the target processor
ifeq "$(TARGET)" ""
TARGET=MAX32665
endif

# Create Target name variables
TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)

# Select 'GCC' or 'IAR' compiler
COMPILER=GCC

# Specify the board used
ifeq "$(BOARD)" ""
BOARD=MAXREFDES178
endif

# This is the path to the CMSIS root directory
ifeq "$(MAXIM_PATH)" ""
LIBS_DIR=../../../Libraries
else
LIBS_DIR=/$(subst \,/,$(subst :,,$(MAXIM_PATH))/Libraries)
endif

ifeq ($(OS),Windows_NT)
MSBLGEN=msblGenWin32.exe
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
MSBLGEN=msblGenLinux
endif
ifeq ($(UNAME_S),Darwin)
MSBLGEN=msblGenOSX
endif
endif

CMSIS_ROOT=$(LIBS_DIR)/CMSIS
ifeq ($(MAKECMDGOALS),sla)
SECURE_BOOT_TOOLS_DIR=$(LIBS_DIR)/../Tools/SBT
CA_SIGN_BUILD=$(SECURE_BOOT_TOOLS_DIR)/bin/sign_app
BUILD_SESSION=$(SECURE_BOOT_TOOLS_DIR)/bin/build_scp_session
TEST_KEY=$(SECURE_BOOT_TOOLS_DIR)/devices/$(TARGET_UC)/keys/maximtestcrk.key
SCP_PACKETS=scp_packets
endif

#Use this for other library make files so they are all based off the same as root as the project
export CMSIS_ROOT

# Source files for this test (add path to VPATH below)
SRCS  = max32666_main.c
SRCS += max32666_accel.c
SRCS += max32666_audio_codec.c
#SRCS += max32666_ble.c
#SRCS += max32666_ble_command.c
#SRCS += max32666_ble_queue.c
SRCS += max32666_data.c
SRCS += max32666_expander.c
#SRCS += max32666_ext_flash.c
#SRCS += max32666_ext_sram.c
SRCS += max32666_fault.c
SRCS += max32666_fonts.c
SRCS += max32666_fuel_gauge.c
SRCS += max32666_i2c.c
SRCS += max32666_lcd.c
SRCS += max32666_pmic.c
#SRCS += max32666_powmon.c
SRCS += max32666_qspi_master.c
SRCS += max32666_sdcard.c
SRCS += max32666_spi_dma.c
SRCS += max32666_timer_led_button.c
SRCS += max32666_touch.c
#SRCS += max32666_usb.c
SRCS += max32666_record.c
SRCS += maxrefdes178_utility.c

ifeq ($(MAKECMDGOALS),sla)
SRCS += sla_header.c
endif

# External library source files
SRCS += bmi160.c

# Where to find source files for this test
VPATH = src
VPATH += ../../maxrefdes178_common
ifeq ($(MAKECMDGOALS),sla)
VPATH += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source
endif
VPATH += ../../external/BMI160_driver

# Where to find header files for this test
IPATH  = include
IPATH += ../../maxrefdes178_common
IPATH += ../../external/BMI160_driver

# Enable assertion checking for development
PROJ_CFLAGS+=-DMXC_ASSERT_ENABLE 

# Enable all warnings
PROJ_CFLAGS+=-Wall
#PROJ_CFLAGS+=-Werror

# Specify the target revision to override default
# "A2" in ASCII
# TARGET_REV=0x4132

# Use this variables to specify and alternate tool path
#TOOL_DIR=/opt/gcc-arm-none-eabi-4_8-2013q4/bin


# Use these variables to add project specific tool options
#PROJ_CFLAGS+=--specs=nano.specs
#PROJ_LDFLAGS+=--specs=nano.specs

# Point this variable to a startup file to override the default file
#STARTUPFILE=start.S

# Set MXC_OPTIMIZE to override the default optimization level 
MXC_OPTIMIZE_CFLAGS=-Ofast
# Use -Og for better debug
#MXC_OPTIMIZE_CFLAGS=-Og

# Point this variable to a linker file to override the default file
LINKER=$(TARGET_LC).ld
#LINKERFILE=$(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/GCC/$(LINKER)
LINKERFILE=maxrefdes178_max32666.ld

ifeq ($(MAKECMDGOALS),sla)
LINKER=$(TARGET_LC)_sla.ld
LINKERFILE=$(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/GCC/$(LINKER)
endif

################################################################################
## Cordio Library Options
#DEBUG           := 1
#TRACE           := 1
#BT_VER          := 8
#INIT_PERIPHERAL := 1
#
## Select either option, or both for combined Host and Controller on single core
#BLE_HOST        := 1
#BLE_CONTROLLER  := 1

################################################################################
# Include external library makefiles here

# Include the BSP
BOARD_DIR=$(LIBS_DIR)/Boards/$(TARGET_UC)/$(BOARD)
include $(BOARD_DIR)/board.mk

# Include the peripheral driver
PERIPH_DRIVER_DIR=$(LIBS_DIR)/PeriphDrivers
include $(PERIPH_DRIVER_DIR)/periphdriver.mk
export PERIPH_DRIVER_DIR

# Include SDHC and FAT32 Library
SDHC_DRIVER_DIR=$(LIBS_DIR)/SDHC
FAT32_DRIVER_DIR=$(SDHC_DRIVER_DIR)/ff13
include $(FAT32_DRIVER_DIR)/fat32.mk
include $(SDHC_DRIVER_DIR)/sdhc.mk

## Include the Cordio Library
#CORDIO_DIR=Cordio
#include $(CORDIO_DIR)/platform/targets/maxim/build/cordio.mk

# Include USB Library
#MAXUSB_DIR=$(LIBS_DIR)/MAXUSB
#include $(MAXUSB_DIR)/maxusb.mk

################################################################################
# Include the rules for building for this target. All other makefiles should be
# included before this one.
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk

libclean: 
	$(MAKE) -f ${PERIPH_DRIVER_DIR}/periphdriver.mk clean.periph

slaclean: 
	rm -r ${SCP_PACKETS}/

distclean: clean libclean

all:
	arm-none-eabi-objcopy $(BUILD_DIR)/$(PROJECT).elf -R .sig -O binary $(BUILD_DIR)/$(PROJECT)_app.bin
	cd $(BUILD_DIR) && $(CURDIR)/../../external/${MSBLGEN} $(BUILD_DIR)/$(PROJECT)_app.bin MAX32666 8192
	cat $(CURDIR)/../../maxrefdes178-AppSwitcher/maxrefdes178_max32666_bootloader.bin $(BUILD_DIR)/$(PROJECT)_app.bin > $(BUILD_DIR)/$(PROJECT).bin

sla: all
	@echo " "
	arm-none-eabi-size --format=berkeley $(BUILD_DIR)/$(PROJECT).elf
	@echo " "
	$(CA_SIGN_BUILD) -c $(TARGET_UC) key_file=$(TEST_KEY) algo=ecdsa header=no ca=$(BUILD_DIR)/$(PROJECT).bin sca=$(BUILD_DIR)/$(PROJECT).sbin
	@echo " "
	arm-none-eabi-objcopy  $(BUILD_DIR)/$(PROJECT).elf --update-section .sig=$(BUILD_DIR)/$(PROJECT).sig
# $(BUILD_SESSION) -c $(TARGET_UC) key_file=$(TEST_KEY) ${SCP_PACKETS} $(BUILD_DIR)/$(PROJECT).sbin
