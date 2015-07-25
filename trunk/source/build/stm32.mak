
#CC						=	$(CC_PATH)/arm-none-eabi-gcc.exe
#AS						=	$(CC_PATH)/arm-none-eabi-as.exe
#AR						=	$(CC_PATH)/arm-none-eabi-ar.exe
#LD						=	$(CC_PATH)/arm-none-eabi-ld.exe
#OC						=	$(CC_PATH)/arm-none-eabi-objcopy.exe
#RL						=	$(CC_PATH)/arm-none-eabi-ranlib.exe

CC						?=	arm-none-eabi-gcc.exe
AS						?=	arm-none-eabi-as.exe
AR						?=	arm-none-eabi-ar.exe
LD						?=	arm-none-eabi-ld.exe
OC						?=	arm-none-eabi-objcopy.exe
RL						?=	arm-none-eabi-ranlib.exe
CP						=	cp
RM						=	rm
MV						=	mv
MD						=	mkdir
RD						=	rmdir
CAT						=	cat
ECHO					=	echo

FWLIB					=	stm32f10x

LD_SCRIPT				=	$(SOURCES_ROOT)/platform/board/$(ARCH)/$(BOARD)/$(COMPILER)/$(ARCH)-$(BOARD).ld
#LD_LIBRARYS_ROOT		?=	-L/usr/local/arm-none-eabi-msys/lib/gcc/arm-none-eabi/4.8.3/thumb
#LD_LIBARAYS_FLAGS		?=	-lgcc_shortwchar

STATIC_LIBRARY			=	.a
DYNAMIC_LIBRARY			=	.a
TARGET_SUFFIX			=	elf


DEBUG					?=	true

ifeq ($(strip $(DEBUG)),true)
CC_DEBUG				=	-D__DEBUG__
endif

CC_FLAGS				=	-ggdb3 -fPIC -c -mcpu=cortex-m3 -mfpu=vfp -mthumb -Wall -Werror -g -O2 $(CC_DEBUG)				\
							-nostdinc -std=c99 -fshort-wchar -finput-charset=UTF-8 -fno-builtin	-D__THUMB__					\
							-D__LITTLE_ENDIAN__ -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DARCH_$(ARCH)						\
							-DBOARD_$(BOARD) 

AS_FLAGS				=	-fPIC -c -mcpu=cortex-m3 -mfpu=vfp -mthumb -Wall -g -x assembler-with-cpp						\
							-mlittle-endian -mfpu=vfp -mthumb -ggdb3 -DARCH_$(ARCH) -DBOARD_$(BOARD) 

LD_FLAGS				=	-Map=$(SOURCES_ROOT)/$(ARCH)-$(BOARD)/$(ARCH)-$(BOARD).map -cref -A cortex-m3					\
							--entry=Reset_Handler -static -T $(LD_SCRIPT) --nostdlib $(LD_LIBRARYS_ROOT)					\
							$(PROJECT_LIBRARYS_ROOT) $(PROJECT_LIBRARYS_FLAGS) $(LD_LIBARAYS_FLAGS)
MK_FLAGS				=	--no-print-directory

MAKE					+=	$(MK_FLAGS)

PROJECT_LIBRARYS_ROOT	=	-L$(OBJECTS_ROOT)
PROJECT_LIBRARYS_FLAGS	=	-lcuser -lapi -lcal -lfw -lcmini

OC_FLAGS				=	-Obinary


CHILDREN				=	$(subst /,, $(shell ls -p | grep /))
SUBDIRS					=	$(CHILDREN)

CURRENT_PATH			=	$(shell pwd)
SOURCES_PATH			=	$(CURRENT_PATH)
RELATIVE_PATH			=	

PATH_FOLDERS			=	$(subst /, ,$(CURRENT_PATH))
CURRENT_FOLDER			=	$(word $(words $(PATH_FOLDERS)), $(PATH_FOLDERS))

BUILD_ROOT				=	$(SOURCES_ROOT)/build
SCRIPTS_ROOT			=	$(SOURCES_ROOT)/script
OBJECTS_ROOT			=	$(SOURCES_ROOT)/$(ARCH)-$(BOARD)
OBJECTS_PATH			=	$(OBJECTS_ROOT)/$(CURRENT_FOLDER)
COMMAND_LIST			=	$(OBJECTS_ROOT)/commands.txt

PROJECT_OBJECTS			=	$(OBJECTS_ROOT)/boot/*.o $(OBJECTS_ROOT)/core/*.o $(OBJECTS_ROOT)/testcode/*.o

TARGETS_ROOT			=	$(OBJECTS_ROOT)
TARGETS_PATH			=	$(TARGETS_ROOT)

C_SOURCE_FILE			=	$(wildcard *.c)
S_SOURCE_FILE			=	$(wildcard *.s)

ARM_SOURCE_FILE			=	$(wildcard ../armcc/*.s)
GTM_SOURCE_FILE			=	$(notdir $(ARM_SOURCE_FILE))
GNU_SOURCE_FILE			=	$(GTM_SOURCE_FILE:%.s=%.asm)
GNU_OBJECT_FILE			=	$(GTM_SOURCE_FILE:%.s=%.o)
GNU_DEPEND_FILE			=	$(GTM_SOURCE_FILE:%.s=%.d)
GNU_DEPENDS				=	$(addprefix $(OBJECTS_PATH)/, $(GNU_DEPEND_FILE)) 
GNU_OBJECTS				=	$(addprefix $(OBJECTS_PATH)/, $(GNU_OBJECT_FILE)) 


C_OBJECT_FILE			=	$(C_SOURCE_FILE:%.c=%.o)
S_OBJECT_FILE			=	$(S_SOURCE_FILE:%.s=%.o)

C_DEPEND_FILE			=	$(C_SOURCE_FILE:%.c=%.d)
S_DEPEND_FILE			=	$(S_SOURCE_FILE:%.s=%.d)

SOURCES					=	$(C_SOURCE_FILE) $(S_SOURCE_FILE)
OBJECTS					=	$(addprefix $(OBJECTS_PATH)/, $(C_OBJECT_FILE)) 													\
							$(addprefix $(OBJECTS_PATH)/, $(S_OBJECT_FILE))
DEPENDS					=	$(addprefix $(OBJECTS_PATH)/, $(C_DEPEND_FILE))

CORE_INCLUDES			:=	-I$(SOURCES_ROOT)/system/core -I$(SOURCES_ROOT)/system/core/stack

OS_INCLUDES				:=	-I$(SOURCES_ROOT)/include -I$(SOURCES_ROOT)/include/$(COMPILER)
STD_INCLUDES			:=	-I$(SOURCES_ROOT)/include/stdc
LIBCM_INCLUDES			:=	-I$(SOURCES_ROOT)/libs/libcmini
LIBCU_INCLUDES			:=	-I$(SOURCES_ROOT)/libs/libcuser
LIBCAL_INCLUDES			:=	-I$(SOURCES_ROOT)/libs/libcal
ARM_CL_INCLUDES			?=	-I/usr/local/arm-none-eabi-msys/lib/gcc/arm-none-eabi/4.8.3/include									\
							-I/usr/local/arm-none-eabi-msys/lib/gcc/arm-none-eabi/4.8.3/include-fixed							\
							-I/usr/local/arm-none-eabi-msys/lib/gcc/arm-none-eabi/4.8.3/install-tools/include
CL_INCLUDES				:=  $(subst :, ,$(ARM_CL_INCLUDES))
ARCH_INCLUDES			:=	-I$(SOURCES_ROOT)/platform/arch/$(ARCH)
BOARD_INCLUDES			:=	-I$(SOURCES_ROOT)/platform/board/$(ARCH)/$(BOARD)													\
							-I$(SOURCES_ROOT)/platform/board/$(ARCH)/$(BOARD)/$(COMPILER)
FW_INCLUDES				:=	-I$(SOURCES_ROOT)/platform/libfw/$(FWLIB)
STM32_INCLUDES			:=	-I$(SOURCES_ROOT)/platform/libfw/$(FWLIB)/Libraries/CMSIS/CM3/CoreSupport							\
							-I$(SOURCES_ROOT)/platform/libfw/$(FWLIB)/Libraries/STM32F10x_StdPeriph_Driver/inc					\
							-I$(SOURCES_ROOT)/platform/libfw/$(FWLIB)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
GLOBAL_INCLUDES			:=	$(STD_INCLUDES) $(OS_INCLUDES) $(CL_INCLUDES) $(FW_INCLUDES) $(BOARD_INCLUDES)						\
							$(ARCH_INCLUDES) $(CORE_INCLUDES) $(STM32_INCLUDES)													\
							$(LIBCM_INCLUDES) $(LIBCU_INCLUDES) $(LIBCAL_INCLUDES)

CONFIG_MAKE				=	$(SOURCES_ROOT)/build/$(ARCH).mak
SUFFIX_MAKE				=	$(SOURCES_ROOT)/build/$(ARCH)/suffix.mak
CREATE_MAKE				=	$(ARCH)-$(BOARD).$(COMPILER)

FOLDER_CONFIG			=	$(wildcard config.mak)

ifeq ($(strip $(FOLDER_CONFIG)),config.mak)
include $(CURRENT_PATH)/config.mak
else
OBJECTS_PATH			=
TARGETS_PATH			=
CONFIGS_PATH			=
endif

CC_INCLUDES				=	$(GLOBAL_INCLUDES) $(LOCAL_INCLUDES)
AS_INCLUDES				=	$(GLOBAL_INCLUDES) $(LOCAL_INCLUDES)
							
.PHONY: $(CONFIGS_PATH) $(SUBDIRS) remove clean remove_config config

all:$(CREATE_MAKE) $(LD_SCRIPT) $(SUFFIX_MAKE)
	@echo "Make $(ARCH)-$(BOARD) now ..."
	@$(ECHO) "@$(MAKE) -f $(CREATE_MAKE)" > $(COMMAND_LIST)
	@$(RM) -f $(TARGETS_ROOT)/*.$(TARGET_SUFFIX)
	@$(RM) -f $(TARGETS_ROOT)/*.$(STATIC_LIBRARY)
	@$(RM) -f $(TARGETS_ROOT)/*.$(DYNAMIC_LIBRARY)
	@$(MAKE) -f $(CREATE_MAKE)

$(CREATE_MAKE):
	@echo "Configure $(ARCH)-$(BOARD) now ..."
	@$(MAKE) config

$(LD_SCRIPT):
	$(error "Can not found file $(LD_SCRIPT)")

$(SUFFIX_MAKE):
	$(error "Can not found file $<")
	
remove_config:
	@for i in $(OBJECTS) $(TARGETS) $(DEPENDS) $(CONFIGS); \
		do $(ECHO) "Delete file [$$i] ..." && $(RM) -f $$i remove_config; \
	done
	@$(ECHO) "Delete file [$(CURDIR)/$(CREATE_MAKE)] ..."
	@$(RM) -f $(CREATE_MAKE)
	@for i in $(SUBDIRS); do $(MAKE) -C $$i -f$(CONFIG_MAKE) remove_config; done
	@$(ECHO) "if [ -d \"$(OBJECTS_PATH)\" ]; then" > ./temp.sh
	@$(ECHO) "	$(ECHO) Remove directory [$(OBJECTS_PATH)]" >> ./temp.sh
	@$(ECHO) "	$(RD) --ignore-fail-on-non-empty $(OBJECTS_PATH)" >> ./temp.sh
	@$(ECHO) "fi" >> ./temp.sh
#	@$(ECHO) "for dir in $(OBJECTS_PATH); do" > ./temp.sh
#	@$(ECHO) "  if [ -d \"$$dir\" ]; then" >> ./temp.sh
#	@$(ECHO) "    $(RD) --ignore-fail-on-non-empty $$dir" >> ./temp.sh
#	@$(ECHO) "  fi;" >> ./temp.sh
#	@$(ECHO) "done" >> ./temp.sh
	@./temp.sh
	@$(RM) -rf ./temp.sh

remove:
	@$(ECHO) "Remove $(ARCH)-$(BOARD) config now ..."
	@$(MAKE) remove_config
	@$(RM) -rf $(OBJECTS_ROOT)

config: $(CONFIGS_PATH) $(OBJECTS_PATH) $(TARGETS_PATH) \
		$(SUBDIRS) $(SOURCES) $(DEPENDS) $(CONFIGS)
	@$(ECHO) "Create file [$(CURDIR)/$(CREATE_MAKE)] ..."
	@$(ECHO) "" > $(CREATE_MAKE)
	@$(ECHO) "CC              = $(CC)" >> $(CREATE_MAKE)
	@$(ECHO) "AS              = $(AS)" >> $(CREATE_MAKE)
	@$(ECHO) "AR              = $(AR)" >> $(CREATE_MAKE)
	@$(ECHO) "LD              = $(LD)" >> $(CREATE_MAKE)
	@$(ECHO) "OC              = $(OC)" >> $(CREATE_MAKE)
	@$(ECHO) "RL              = $(RL)" >> $(CREATE_MAKE)
	@$(ECHO) "CP              = $(CP)" >> $(CREATE_MAKE)
	@$(ECHO) "RM              = $(RM)" >> $(CREATE_MAKE)
	@$(ECHO) "MD              = $(MD)" >> $(CREATE_MAKE)
	@$(ECHO) "ECHO            = $(ECHO)" >> $(CREATE_MAKE)
	@$(ECHO) "CC_FLAGS        = $(CC_FLAGS)" >> $(CREATE_MAKE)
	@$(ECHO) "AS_FLAGS        = $(AS_FLAGS)" >> $(CREATE_MAKE)
	@$(ECHO) "LD_FLAGS        = $(LD_FLAGS)" >> $(CREATE_MAKE)
	@$(ECHO) "CC_INCLUDES     = \\" >> $(CREATE_MAKE)
	@for i in $(CC_INCLUDES); do $(ECHO) "                 $$i\\" >> $(CREATE_MAKE); done
	@$(ECHO) "" >> $(CREATE_MAKE)
	@$(ECHO) "AS_INCLUDES     = \\" >> $(CREATE_MAKE)
	@for i in $(AS_INCLUDES); do $(ECHO) "                 $$i\\" >> $(CREATE_MAKE); done
	@$(ECHO) "" >> $(CREATE_MAKE)
	@$(ECHO) "SCRIPTS_ROOT    = $(SCRIPTS_ROOT)" >> $(CREATE_MAKE)
	@$(ECHO) "SOURCES_ROOT    = $(SOURCES_ROOT)" >> $(CREATE_MAKE)
	@$(ECHO) "OBJECTS_ROOT    = $(OBJECTS_ROOT)" >> $(CREATE_MAKE)
	@$(ECHO) "OBJECTS_PATH    = $(OBJECTS_PATH)" >> $(CREATE_MAKE)
	@$(ECHO) "TARGETS_PATH    = $(TARGETS_PATH)" >> $(CREATE_MAKE)
	@$(ECHO) "COMMAND_LIST    = $(COMMAND_LIST)" >> $(CREATE_MAKE)
	@$(ECHO) "SUBDIRS         = $(SUBDIRS)" >> $(CREATE_MAKE)
	@$(ECHO) "MAKEFILE        = $(CREATE_MAKE)" >>  $(CREATE_MAKE)
	@$(ECHO) "PROJECT_OBJECTS = $(PROJECT_OBJECTS)" >>  $(CREATE_MAKE)
	@$(ECHO) "TARGETS         = $(TARGETS)" >> $(CREATE_MAKE)
	@$(ECHO) "OBJECTS         = \\" >> $(CREATE_MAKE)
	@for i in $(OBJECTS); do $(ECHO) "                 $$i\\" >> $(CREATE_MAKE); done
	@$(ECHO) "" >> $(CREATE_MAKE)
	@$(ECHO) ".PHONY: $(SUBDIRS) clean" >> $(CREATE_MAKE)
	@$(ECHO) "all: \\" >> $(CREATE_MAKE)
	@$(ECHO) "    $(SUBDIRS)\\" >> $(CREATE_MAKE)
	@for i in $(OBJECTS); do $(ECHO) "    $$i\\" >> $(CREATE_MAKE); done
	@$(ECHO) "    $(TARGETS)\\" >> $(CREATE_MAKE)
	@$(ECHO) "" >> $(CREATE_MAKE)
	@for i in $(DEPENDS); do $(ECHO) "include $$i" >> $(CREATE_MAKE); done
	@$(ECHO) "include $(SUFFIX_MAKE)" >> $(CREATE_MAKE)

$(SUBDIRS):
	@$(MAKE) -C $@ -f $(CONFIG_MAKE) config

$(CONFIGS_PATH):
	@$(ECHO) "Create directory [$@] ..."
	@$(MD) -p $@
	
$(TARGETS_PATH):
	@$(ECHO) "Create directory [$@] ..."
	@$(MD) -p $@

$(OBJECTS_PATH):
	@$(ECHO) "Create directory [$@] ..."
	@$(MD) -p $@
	
$(CONFIGS_PATH)/$(COMPILER):
	@$(ECHO) "Create directory [$@] ..."
	@$(MD) -p $@

%.asm: ../armcc/%.s
	@$(ECHO) "Create file [$@] ..."
	@$(SCRIPTS_ROOT)/arm2gnu.sh $< $@
	
%.asm: $(SOURCES_PATH)/%.s
	@$(ECHO) "Create file [$@] ..."
	@$(CP) $< $@

$(OBJECTS_PATH)/%.d: %.c $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) '$(OBJECTS_PATH)/' | tr -d '\n' > $@
	$(CC) $(CC_FLAGS) $(CC_INCLUDES) -MM $< >> $@

$(OBJECTS_PATH)/%.d: %.s $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) '$(OBJECTS_PATH)/' | tr -d '\n' > $@
	$(CC) $(AS_FLAGS) $(AS_INCLUDES) -MM $< >> $@
	
$(OBJECTS_PATH)/%.d: %.asm $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) '$(OBJECTS_PATH)/' | tr -d '\n' > $@
	$(CC) $(AS_FLAGS) $(AS_INCLUDES) -MM $< >> $@
	
$(CONFIGS_PATH)/%.h: $(SOURCES_ROOT)/%.ini $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(SCRIPTS_ROOT)/ini2ver.sh $< $@

$(CONFIGS_PATH)/%_enum.h: $(CONFIGS_PATH)/%.ini $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(SCRIPTS_ROOT)/ini2enum.sh $< $@

$(CONFIGS_PATH)/%_name.h: $(CONFIGS_PATH)/%.ini $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(SCRIPTS_ROOT)/ini2name.sh $< $@

$(CONFIGS_PATH)/%.inc: $(CONFIGS_PATH)/%.ini $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(SCRIPTS_ROOT)/ini2inc.$(COMPILER).sh $< $@

$(CONFIGS_PATH)/$(COMPILER)/%_enum.inc: $(CONFIGS_PATH)/%.ini $(CONFIGS_PATH)/$(COMPILER) $(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(SCRIPTS_ROOT)/ini2inc.$(COMPILER).sh $< $@

$(CONFIGS_PATH)/%.h:$(CONFIG_MAKE)
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) "#define    BOARD_NAME_STRING    \"$(ARCH)-$(BOARD)\"" > $@
clean:
	@$(MAKE) -f $(CREATE_MAKE) clean

	
