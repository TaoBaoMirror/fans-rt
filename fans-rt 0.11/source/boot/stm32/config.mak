SOURCES_PATH_gcc	=	$(SOURCES_ROOT)/platform/libfw/stm32f10x/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7
SOURCES_PATH		=	$(SOURCES_PATH_$(COMPILER))
OBJECTS_tiny		=	startup_stm32f10x_ld.asm $(OBJECTS_PATH)/startup_stm32f10x_ld.o
OBJECTS_mini		=	startup_stm32f10x_md.asm $(OBJECTS_PATH)/startup_stm32f10x_md.o
OBJECTS_lite		=	startup_stm32f10x_md.asm $(OBJECTS_PATH)/startup_stm32f10x_md.o
OBJECTS_huge		=	startup_stm32f10x_hd.asm $(OBJECTS_PATH)/startup_stm32f10x_hd.o
OBJECTS_soft		=	startup_stm32f10x_hd.asm $(OBJECTS_PATH)/startup_stm32f10x_hd.o
OBJECTS				=	$(OBJECTS_$(BOARD))
SOURCES_tiny		=	startup_stm32f10x_ld.asm
SOURCES_mini		=	startup_stm32f10x_md.asm
SOURCES_lite		=	startup_stm32f10x_md.asm
SOURCES_huge		=	startup_stm32f10x_hd.asm
SOURCES_soft		=	startup_stm32f10x_hd.asm
SOURCES				=	$(SOURCES_$(BOARD))
DEPENDS				=
TARGETS				=
OBJECTS_PATH		=	$(OBJECTS_ROOT)/boot
CONFIGS_PATH		=
TARGETS_PATH		=


