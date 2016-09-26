OPENOCD_PATH		?= /opt/gnuarmeclipse/openocd/0.10.0-201510281129-dev
OPENOCD           	?= $(OPENOCD_PATH)/bin/openocd
OPENOCD_INTERFACE 	?= $(OPENOCD_PATH)/scripts/interface/stlink-v2.cfg


ifdef TAG
REV					= C
else
REV					= B
endif


FORMAT = ihex

BUILDDIR = Build



OPENOCD_TARGET_2     ?= 	$(OPENOCD_PATH)/scripts/target/stm32f3x.cfg
OPENOCD_TARGET_1     ?= 	$(OPENOCD_PATH)/scripts/target/stm32f1x.cfg
OPENOCD_TARGET_0     ?= 	$(OPENOCD_PATH)/scripts/target/stm32f0x.cfg

ifeq ($(strip $(REV)),A)
	TARGET 				= 	Tag
	CPU					=	f1
	PROCESSOR			=	-mthumb -mcpu=cortex-m3 -DTHUMB -DHSE_VALUE="((uint32_t)16000000)" -DSTM32F10X_MD -DARM_MATH_CM3 -DENABLE_EKF
	#PROCESSOR 			+= -DEMPL -DUSE_DMP -DMPU9250 -DMPL_LOG_NDEBUG=1 -DEMPL_TARGET_STM32F1xx #-DREMOVE_LOGGING
	LINKERFILE			=	Linkers/stm32f103.ld
else ifeq ($(strip $(REV)),B)
	TARGET 				= 	Anchor
	CPU					=	f0
	PROCESSOR			=	-mthumb -mcpu=cortex-m0 -DSTM32F072xB -DTHUMB
	LINKERFILE			=	Linkers/STM32F072CBUx_FLASH.ld
else ifeq ($(strip $(REV)),C)
	TARGET 				= 	Tag
	CPU 				= 	f3
	PROCESSOR 			= 	-mthumb -mcpu=cortex-m4 -DTHUMB -DHSE_VALUE="((uint32_t)16000000)" -DSTM32F302xC -DARM_MATH_CM4 
	#PROCESSOR 			+= -DEMPL -DUSE_DMP -DMPU9250 -DMPL_LOG_NDEBUG=1 -DEMPL_TARGET_STM32F3xx #-DREMOVE_LOGGING -DSTM32F303xC -DENABLE_EKF
	LINKERFILE			=	Linkers/STM32F302CCTx_FLASH.ld
else
    $(error Rev.$(REV) unknown)
endif

INCLUDES =  -IInc
INCLUDES += -IInc/$(CPU)
INCLUDES += -IDrivers/CMSIS/Include


# Platform specific files
ifeq ($(strip $(REV)),A)
	SRC 	= 	Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c

	STARTUP  = 	Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/startup_stm32f103xb.S

	INCLUDES += -ILibraries/CMSIS/CM3/CoreSupport
	INCLUDES += -ILibraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
	INCLUDES += -ILibraries/STM32F10x_StdPeriph_Driver/inc

	PERIP 	+= adc crc flash rtc exti gpio i2c rcc spi tim usart
	SRC 	+= $(foreach mod, $(PERIP), Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_$(mod).c)
	SRC 	+= Libraries/STM32F10x_StdPeriph_Driver/src/misc.c

	SRC		+= Source/$(CPU)/stm32f10x_it.c
	CPPSRC	= Source/ekf.cpp

	# AUMAT Library
	INCLUDES += -ILibraries/aumat
	SRC		 += Libraries/aumat/matcore.c
	SRC		 += Libraries/aumat/matinv.c
	

else ifeq ($(strip $(REV)),B)
	SRC 	 	=	Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c

	STARTUP 	= 	Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f072xb.S
	
	INCLUDES 	+= -IDrivers/STM32F0xx_HAL_Driver/Inc
	INCLUDES 	+= -IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy
	INCLUDES 	+= -IDrivers/CMSIS/Device/ST/STM32F0xx/Include

	SRC 		+= Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c

	PERIP 		+= adc adc_ex crc crc_ex cortex dma flash flash_ex gpio i2c i2c_ex pcd pcd_ex pwr pwr_ex rcc rcc_ex spi spi_ex tim tim_ex 	
	SRC 		+= $(foreach mod, $(PERIP), Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_$(mod).c)

	SRC			+= Src/$(CPU)/stm32f0xx_hal_msp.c
	SRC			+= Src/$(CPU)/stm32f0xx_it.c

else ifeq ($(strip $(REV)),C)
	SRC 		= 	Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c

	STARTUP  	= 	Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f302xc.S

	INCLUDES 	+= -IDrivers/STM32F3xx_HAL_Driver/Inc
	INCLUDES 	+= -IDrivers/STM32F3xx_HAL_Driver/Inc/Legacy
	INCLUDES 	+= -IDrivers/CMSIS/Device/ST/STM32F3xx/Include

	SRC 		+= Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c

	PERIP 		+= adc adc_ex crc crc_ex cortex dma flash flash_ex gpio i2c i2c_ex pcd pcd_ex pwr pwr_ex rcc rcc_ex spi spi_ex tim tim_ex 	
	SRC 		+= $(foreach mod, $(PERIP), Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_$(mod).c)

	SRC			+= Src/$(CPU)/stm32f3xx_hal_msp.c
	SRC			+= Src/$(CPU)/stm32f3xx_it.c
	
	#CPPSRC		= Source/ekf.cpp


	# AUMAT Library
	#INCLUDES 	+= -ILibraries/aumat
	#SRC		+= Libraries/aumat/matcore.c
	#SRC		+= Libraries/aumat/matinv.c

	# MPU Library
	#SRC			+= Source/$(CPU)/mpu_i2c.c
	#SRC			+= Source/mpu9250.c

	#INCLUDES 	+= -ILibraries/MPU_Driver/driver/eMPL
	#INCLUDES 	+= -ILibraries/MPU_Driver/driver/include
	#INCLUDES 	+= -ILibraries/MPU_Driver/eMPL-hal
	#INCLUDES 	+= -ILibraries/MPU_Driver/mllite
	#INCLUDES 	+= -ILibraries/MPU_Driver/mpl

	#SRC		+= Libraries/MPU_Driver/driver/eMPL/inv_mpu.c
	#SRC		+= Libraries/MPU_Driver/driver/eMPL/inv_mpu_dmp_motion_driver.c
	#SRC		+= Libraries/MPU_Driver/eMPL-hal/eMPL_outputs.c
	#SRC 		+= Libraries/MPU_Driver/mllite/data_builder.c
	#SRC 		+= Libraries/MPU_Driver/mllite/hal_outputs.c
	#SRC 		+= Libraries/MPU_Driver/mllite/message_layer.c
	#SRC 		+= Libraries/MPU_Driver/mllite/mlmath.c
	#SRC 		+= Libraries/MPU_Driver/mllite/ml_math_func.c
	#SRC 		+= Libraries/MPU_Driver/mllite/mpl.c
	#SRC 		+= Libraries/MPU_Driver/mllite/results_holder.c
	#SRC 		+= Libraries/MPU_Driver/mllite/start_manager.c
	#SRC 		+= Libraries/MPU_Driver/mllite/storage_manager.c


	#LIBS 		= -LLibraries/MPU_Driver/mpl -llibmplmpu
endif

# USB Middleware files
INCLUDES 	+= -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc
SRC 		+= Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
SRC 		+= Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
SRC 		+= Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c

INCLUDES 	+= -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
SRC 		+= Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c

# USB setup files and conf
SRC 		+= Src/usbd_desc.c
SRC 		+= Src/usbd_cdc_if.c
SRC 		+= Src/usbd_conf.c
SRC 		+= Src/usb_device.c

#Error handler
SRC 		+= Src/error.c


# User objects
CPPSRC 		+= Src/main.cpp
CPPSRC 		+= Src/deca.cpp
CPPSRC 		+= Src/decaTime.cpp
CPPSRC 		+= Src/device.cpp
CPPSRC 		+= Src/ranging.cpp
CPPSRC 		+= Src/mac.cpp
SRC			+= Src/log.c
SRC			+= Src/$(CPU)/sys.c
SRC			+= Src/$(CPU)/rng.c

# Periph files
SRC 		+= Src/dma.c
SRC 		+= Src/gpio.c
SRC 		+= Src/i2c.c
SRC 		+= Src/spi.c
SRC 		+= Src/tim.c


# Decawave files
INCLUDES 	+= -IDrivers/decadriver
SRC 		+= Drivers/decadriver/deca_device.c
SRC 		+= Drivers/decadriver/deca_params_init.c
SRC			+= Drivers/decadriver/deca_range_tables.c
SRC 		+= Src/decawave.c


CFLAGS 		+= $(PROCESSOR) -DUSE_STDPERIPH_DRIVER $(INCLUDES) -Os -g0 -Wall -fdiagnostics-color=auto

# flags only for C++ (arm-none-eabi-g++)
# CPPFLAGS = -fno-rtti -fno-exceptions
CPPFLAGS = -std=c++11  -Wno-pmf-conversions

# flags only for C (arm-none-eabi-gcc)
CONLYFLAGS 	= -Wno-pointer-sign -std=gnu11

#LDFLAGS 	=  $(PROCESSOR) --specs=nano.specs --specs=rdimon.specs -lc -lrdimon -u _printf_float
LDFLAGS 	=  $(PROCESSOR) --specs=nano.specs --specs=nosys.specs -lm -lgcc -lc -u _printf_float
LDFLAGS 	+= -T$(LINKERFILE)

ASFLAGS 	= $(PROCESSOR) -x assembler-with-cpp 



SHELL = sh
CC = arm-none-eabi-gcc
CPP = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
NM = arm-none-eabi-nm
REMOVE = rm -f
COPY = cp


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_ARM = "Compiling C (ARM-only):"
MSG_COMPILINGCPP = Compiling C++:
MSG_COMPILINGCPP_ARM = "Compiling C++ (ARM-only):"
MSG_ASSEMBLING = Assembling:
MSG_ASSEMBLING_ARM = "Assembling (ARM-only):"
MSG_CLEANING = Cleaning project:


ASRC = $(STARTUP)

# Define all object files.
COBJ      = $(SRC:%.c=$(BUILDDIR)/$(TARGET)/%.o) 
AOBJ      = $(ASRC:%.S=$(BUILDDIR)/$(TARGET)/%.o)
COBJARM   = $(SRCARM:%.c=$(BUILDDIR)/$(TARGET)/%.o)
AOBJARM   = $(ASRCARM:%.S=$(BUILDDIR)/$(TARGET)/%.o)
CPPOBJ    = $(CPPSRC:%.cpp=$(BUILDDIR)/$(TARGET)/%.o) 
CPPOBJARM = $(CPPSRCARM:%.cpp=$(BUILDDIR)/$(TARGET)/%.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(ASRCARM:.S=.lst) $(SRC:.c=.lst) $(SRCARM:.c=.lst)
LST += $(CPPSRC:.cpp=.lst) $(CPPSRCARM:.cpp=.lst)

# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

#CFLAGS += $(GENDEPFLAGS)


# Default target.
all: begin gccversion sizebefore build sizeafter finished end

tag: 
	$(MAKE) TAG=1

anchor: 
	$(MAKE)
	

build: none-eabi hex lss sym bin dfu

none-eabi: $(BUILDDIR)/$(TARGET).none-eabi
hex: $(BUILDDIR)/$(TARGET).hex
lss: $(BUILDDIR)/$(TARGET).lss 
sym: $(BUILDDIR)/$(TARGET).sym
bin: $(BUILDDIR)/$(TARGET).bin
dfu: $(BUILDDIR)/$(TARGET).dfu

# Eye candy.
begin:
	@echo $(REV)
	@echo
	@mkdir -p $(BUILDDIR)
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(BUILDDIR)/$(TARGET).hex
ELFSIZE = $(SIZE) -A $(BUILDDIR)/$(TARGET).none-eabi
sizebefore:
	@if [ -f $(BUILDDIR)/$(TARGET).none-eabi ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(BUILDDIR)/$(TARGET).none-eabi ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi


# Display compiler version information.
gccversion : 
	@$(CC) --version


# Create final output files (.hex, .eep) from ELF output file.
# TODO: handling the .eeprom-section should be redundant
%.hex: %.none-eabi
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) $< $@

%.bin: %.none-eabi
	$(OBJCOPY) $^ -O binary $@


# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.none-eabi
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -C $< > $@


# Create a symbol table from ELF output file.
%.sym: %.none-eabi
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@

%.dfu: %.bin
	python2 Scripts/dfu-convert.py -b 0x8000000:$^ $@


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).none-eabi
.PRECIOUS : $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM)
%.none-eabi:  $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM)
	@echo
	@echo $(MSG_LINKING) $@
#	$(CC) $(THUMB) $(CFLAGS) $(CONLYFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@ $(LDFLAGS) $(LIBS)
	$(CPP) $(THUMB) $(CFLAGS) $(CPPFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@ $(LDFLAGS) $(LIBS)

# Compile: create object files from C source files. ARM/Thumb
$(COBJ) : $(BUILDDIR)/$(TARGET)/%.o : %.c
	@echo 
	@mkdir -p $(@D)
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(CFLAGS) $(CONLYFLAGS) $< -o $@ 

# Compile: create object files from C source files. ARM-only
$(COBJARM) : $(BUILDDIR)/$(TARGET)/%.o : %.c
	@echo
	@mkdir -p $(@D)
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(CFLAGS) $(CONLYFLAGS) $< -o $@ 

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJ) : $(BUILDDIR)/$(TARGET)/%.o : %.cpp
	@echo
	@mkdir -p $(@D)
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(CFLAGS) $(CPPFLAGS) $< -o $@ 

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARM) : $(BUILDDIR)/$(TARGET)/%.o : %.cpp
	@echo
	@mkdir -p $(@D)
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ 


# Compile: create assembler files from C source files. ARM/Thumb
## does not work - TODO - hints welcome
##$(COBJ) : %.s : %.c
##	$(CC) $(THUMB) -S $(CFLAGS) $< -o $@


# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJ) : $(BUILDDIR)/$(TARGET)/%.o : %.S
	@echo
	@mkdir -p $(@D)
	@echo $(MSG_ASSEMBLING) $<
	$(CPP) -c $(THUMB) $(ASFLAGS) $< -o $@


# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM) : $(BUILDDIR)/$(TARGET)/%.o : %.S
	@echo
	@mkdir -p $(@D)
	@echo $(MSG_ASSEMBLING_ARM) $<
	$(CPP) -c $(ASFLAGS) $< -o $@

flash-tag: tag
	#python2 Scripts/jump-dfu.py
	dfu-util -d 0483:df11 -a 0 -D $(BUILDDIR)/Tag.dfu -s :leave

flash-tag-swd: tag
	$(OPENOCD) -d2 -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET_1) -c init -c targets -c "reset halt" \
	           -c "flash write_image erase "$(BUILDDIR)/"Tag.hex" -c "verify_image "$(BUILDDIR)/"Tag.hex" -c "reset run" -c shutdown

openocd:
	$(OPENOCD) -d2 -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET_0) 
	#-c init -c targets

flash-anchor: anchor
	#python2 Scripts/jump-dfu.py
	dfu-util -d 0483:df11 -a 0 -D $(BUILDDIR)/Anchor.dfu -s :leave

flash-anchor-swd: anchor
	$(OPENOCD) -d2 -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET_0) -c init -c targets -c "reset halt" \
	           -c "flash write_image erase "$(BUILDDIR)/$(TARGET)".hex" -c "verify_image "$(BUILDDIR)/$(TARGET)".hex" -c "reset run" -c shutdown


# Target: clean project.
clean: begin clean_list finished end


clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) -r $(BUILDDIR)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRCARM:.c=.s)
	$(REMOVE) $(SRCARM:.c=.d)
	$(REMOVE) $(CPPSRC:.cpp=.s) 
	$(REMOVE) $(CPPSRC:.cpp=.d)
	$(REMOVE) $(CPPSRCARM:.cpp=.s) 
	$(REMOVE) $(CPPSRCARM:.cpp=.d)
	$(REMOVE) .dep/*


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build none-eabi hex lss sym bin dfu flash-tag flash-anchor clean clean_list program \
tag anchor
