##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.19.2] date: [Sat Apr 08 00:06:56 CEST 2023] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = project


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/main.c \
Core/Src/stm32f4xx_it.c \
Core/Src/stm32f4xx_hal_msp.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
Core/Src/system_stm32f4xx.c \
MBEDTLS/App/mbedtls.c \
Middlewares/Third_Party/mbedTLS/library/aes.c \
Middlewares/Third_Party/mbedTLS/library/asn1parse.c \
Middlewares/Third_Party/mbedTLS/library/asn1write.c \
Middlewares/Third_Party/mbedTLS/library/base64.c \
Middlewares/Third_Party/mbedTLS/library/bignum.c \
Middlewares/Third_Party/mbedTLS/library/camellia.c \
Middlewares/Third_Party/mbedTLS/library/certs.c \
Middlewares/Third_Party/mbedTLS/library/chacha20.c \
Middlewares/Third_Party/mbedTLS/library/chachapoly.c \
Middlewares/Third_Party/mbedTLS/library/cipher.c \
Middlewares/Third_Party/mbedTLS/library/cipher_wrap.c \
Middlewares/Third_Party/mbedTLS/library/ctr_drbg.c \
Middlewares/Third_Party/mbedTLS/library/dhm.c \
Middlewares/Third_Party/mbedTLS/library/ecdh.c \
Middlewares/Third_Party/mbedTLS/library/ecdsa.c \
Middlewares/Third_Party/mbedTLS/library/ecp.c \
Middlewares/Third_Party/mbedTLS/library/ecp_curves.c \
Middlewares/Third_Party/mbedTLS/library/entropy.c \
Middlewares/Third_Party/mbedTLS/library/entropy_poll.c \
Middlewares/Third_Party/mbedTLS/library/gcm.c \
Middlewares/Third_Party/mbedTLS/library/hkdf.c \
Middlewares/Third_Party/mbedTLS/library/md.c \
Middlewares/Third_Party/mbedTLS/library/md_wrap.c \
Middlewares/Third_Party/mbedTLS/library/memory_buffer_alloc.c \
Middlewares/Third_Party/mbedTLS/library/oid.c \
Middlewares/Third_Party/mbedTLS/library/pem.c \
Middlewares/Third_Party/mbedTLS/library/pk.c \
Middlewares/Third_Party/mbedTLS/library/pkparse.c \
Middlewares/Third_Party/mbedTLS/library/pk_wrap.c \
Middlewares/Third_Party/mbedTLS/library/platform.c \
Middlewares/Third_Party/mbedTLS/library/platform_util.c \
Middlewares/Third_Party/mbedTLS/library/poly1305.c \
Middlewares/Third_Party/mbedTLS/library/rsa_internal.c \
Middlewares/Third_Party/mbedTLS/library/sha256.c \
Middlewares/Third_Party/mbedTLS/library/sha512.c \
Middlewares/Third_Party/mbedTLS/library/ssl_ciphersuites.c \
Middlewares/Third_Party/mbedTLS/library/version_features.c \
Middlewares/Third_Party/mbedTLS/library/x509.c \
Middlewares/Third_Party/mbedTLS/library/x509_crt.c

# ASM sources
ASM_SOURCES =  \
startup_stm32f401xe.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F401xE \
-DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include \
-IMBEDTLS/App \
-IMiddlewares/Third_Party/mbedTLS/include/mbedtls \
-IMiddlewares/Third_Party/mbedTLS/include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F401RETx_FLASH.ld
LDSCRIPT = link_script.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

flash: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
