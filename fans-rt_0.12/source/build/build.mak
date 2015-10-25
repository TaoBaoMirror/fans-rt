CHARSET_NAME			?=	gb2312
CURRENT_PATH			=	$(shell pwd)
PATH_SPLITS				=	$(subst /, ,$(CURRENT_PATH))
CURRENT_FOLDER			=	$(word $(words $(PATH_SPLITS)), $(PATH_SPLITS))

BUILD_ROOT				=	$(SOURCES_ROOT)/build
SCRIPTS_ROOT			=	$(SOURCES_ROOT)/scriptTO
FONTSET_ROOT			=	$(SOURCES_ROOT)/../resource/fonts
OBJECTS_ROOT			?=	$(SOURCES_ROOT)/../objects/$(ARCH)/$(COMPILER)
DEPENDS_ROOT			?=	$(SOURCES_ROOT)/../depends/$(ARCH)/$(COMPILER)
COMMAND_LIST			?=	$(OBJECTS_ROOT)/commands.txt
TARGETS_ROOT			=	$(SOURCES_ROOT)/../targets/$(ARCH)/$(COMPILER)
SHAREDS_ROOT			=	$(SOURCES_ROOT)/../libs/$(ARCH)/$(COMPILER)

SOURCES_PATH			=	$(CURRENT_PATH)
TARGETS_PATH			=	$(TARGETS_ROOT)/$(CURRENT_FOLDER)
OBJECTS_PATH			?=	$(OBJECTS_ROOT)/$(CURRENT_FOLDER)
DEPENDS_PATH			?=	$(DEPENDS_ROOT)/$(CURRENT_FOLDER)

CONFIGS_MAKE			=	$(wildcard config.mak)
TOOLSET_MAKE			=	$(BUILD_ROOT)/tool_config.mak
PREFIXS_MAKE			=	$(SOURCES_ROOT)/build/build.mak
SUFFIXS_ARCH			=	$(SOURCES_ROOT)/build/$(ARCH)/$(ARCH)_suffix.mak
SUFFIXS_HOST			=	$(SOURCES_ROOT)/build/host_suffix.mak
SUFFIXS_MAKE			=	$(SUFFIXS_ARCH)
CURRENT_MAKE			=	$(ARCH)-$(BOARD).$(COMPILER)

SUBDIRS					=	$(subst /,, $(shell ls -p | grep /))

C_SOURCE_FILE			=	$(wildcard *.c)
S_SOURCE_FILE			=	$(wildcard *.s)

C_OBJECT_FILE			=	$(C_SOURCE_FILE:%.c=%.o)
S_OBJECT_FILE			=	$(S_SOURCE_FILE:%.s=%.o)
S_BINARY_FILE			=	$(S_SOURCE_FILE:%.s=%.bin)

C_DEPEND_FILE			=	$(C_SOURCE_FILE:%.c=%.d)
S_DEPEND_FILE			=	$(S_SOURCE_FILE:%.s=%.d)

C_TARGET_HOST			=	$(C_SOURCE_FILE:%.c=%.exe)

SOURCES					=	$(C_SOURCE_FILE) $(S_SOURCE_FILE)
COFFOBJ					=	$(addprefix $(OBJECTS_PATH)/, $(C_OBJECT_FILE)) 													\
							$(addprefix $(OBJECTS_PATH)/, $(S_OBJECT_FILE))
OBJECTS					=	$(COFFOBJ)
BINARYS					=	$(addprefix $(OBJECTS_PATH)/, $(S_BINARY_FILE))
DEPENDS					=	$(addprefix $(DEPENDS_PATH)/, $(C_DEPEND_FILE))


ifeq ($(strip $(CONFIGS_MAKE)),config.mak)
include $(CONFIGS_MAKE)
endif

ifneq ($(wildcard $(TOOLSET_MAKE)),)
include $(TOOLSET_MAKE)
endif

include $(BUILD_ROOT)/$(ARCH)/$(ARCH)_config.mak

ifeq ($(SUFFIXS_MAKE),$(SUFFIXS_HOST))
#TARGETS_PATH			=	$(TOOLSET_PATH)/targets
#OBJECTS_PATH			=	$(TOOLSET_PATH)/objects
#DEPENDS_PATH			=	$(TOOLSET_PATH)/depends
TOOLSET_ROOT			=	$(SOURCES_ROOT)/../targets/tools
TOOLSET_PATH			=	$(TOOLSET_ROOT)/$(CURRENT_FOLDER)
TOOLSET_NAME			=	$(shell tr '[a-z]' '[A-Z]' <<< "$(CURRENT_FOLDER)" | tr -d '\n')
endif


ifneq ($(LIBFILE),)
ifneq ($(LIBPATH),)	
LIBRARY					=	$(shell echo $(LIBFILE),$(LIBPATH))
else
LIBRARY					=	$(shell echo $(LIBFILE))
endif
else#####LIBFILE######
ifneq ($(LIBPATH),)	
LIBRARY					=	$(shell echo $(LIBPATH))
endif
endif

.PHONY: $(CONFIGS_PATH) $(SUBDIRS) $(TOOLSET_MAKE) remove clean remove_config config run


all:$(TOOLSET_MAKE) $(CURRENT_MAKE) $(PREFIXS_MAKE) $(SUFFIXS_MAKE)
	$(COMMAND_SHOW)$(ECHO) "Building $(ARCH)-$(BOARD) now ..."
	$(COMMAND_SHOW)$(MAKE) -f $(CURRENT_MAKE)

$(CURRENT_MAKE):
	$(COMMAND_SHOW)$(ECHO) "Configure $(ARCH)-$(BOARD) now ..."
ifeq ($(CURRENT_PATH),$(SOURCES_ROOT))
	$(COMMAND_SHOW)$(ECHO) "" > $(TOOLSET_MAKE)
endif
	$(COMMAND_SHOW)$(MAKE) config

$(SUBDIRS):
	$(COMMAND_SHOW)$(MAKE) -C $@ -f $(PREFIXS_MAKE) config SOURCES_ROOT=$(SOURCES_ROOT) ARCH=$(ARCH) BOARD=$(BOARD) COMPILER=$(COMPILER)

$(TOOLSET_ROOT):
	$(COMMAND_SHOW)$(ECHO) "Create config path directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@
	
$(CONFIGS_PATH):
	$(COMMAND_SHOW)$(ECHO) "Create config path directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@

$(TARGETS_PATH):
	$(COMMAND_SHOW)$(ECHO) "Create target path directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@

$(OBJECTS_ROOT):
	$(COMMAND_SHOW)$(ECHO) "Create object root directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@

$(OBJECTS_PATH):
	$(COMMAND_SHOW)$(ECHO) "Create object path directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@

$(DEPENDS_PATH):
	$(COMMAND_SHOW)$(ECHO) "Create depend path directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@

$(CONFIGS_PATH)/$(COMPILER):
	$(COMMAND_SHOW)$(ECHO) "Create compiler directory [$@] ..."
	$(COMMAND_SHOW)$(MD) -p $@
	
$(CONFIGS_PATH)/sources.mak:
	$(COMMAND_SHOW)$(ECHO) "BINARYS                = \\" >> $(@)
	$(COMMAND_SHOW)$(FOR) i in $(BINARYS); do $(ECHO) "                 $$i\\" >> $(@); done
	$(COMMAND_SHOW)$(ECHO) "" >> $(@)
	
$(DEPENDS_PATH)/%.d: %.c $(CONFIGS_MAKE)
	$(COMMAND_SHOW)$(ECHO) "Create depend file [$@] ..."
	$(COMMAND_SHOW)$(ECHO) '$(DEPENDS_PATH)/' | tr -d '\n' > $@
	$(COMMAND_SHOW)$(CC) $(CC_FLAGS) $(CC_INCLUDES) $(INCLUDE_USER) -MM $< >> $@

config: $(CONFIGS_PATH) $(OBJECTS_ROOT) $(OBJECTS_PATH) $(TARGETS_PATH) $(DEPENDS_PATH) $(TOOLSET_ROOT) \
		$(SUBDIRS) $(SOURCES) $(HEADERS) $(DEPENDS) $(CONFIGS)
	$(COMMAND_SHOW)$(ECHO) "Create file [$(CURRENT_PATH)/$(CURRENT_MAKE)] ..."
	$(COMMAND_SHOW)$(ECHO) "" > $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "BUILD_ROOT             = $(BUILD_ROOT)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TOOLSET_ROOT           = $(TOOLSET_ROOT)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "SCRIPTS_ROOT           = $(SCRIPTS_ROOT)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "SOURCES_ROOT           = $(SOURCES_ROOT)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "OBJECTS_ROOT           = $(OBJECTS_ROOT)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "FONTSET_ROOT           = $(FONTSET_ROOT)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "CURRENT_PATH           = $(CURRENT_PATH)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "OBJECTS_PATH           = $(OBJECTS_PATH)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TARGETS_PATH           = $(TARGETS_PATH)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TOOLSET_NAME		   = $(TOOLSET_NAME)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TOOLSET_PATH		   = $(TOOLSET_PATH)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TOOLSET_MAKE		   = $(TOOLSET_MAKE)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "COMMAND_LIST           = $(COMMAND_LIST)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "CURRENT_FOLDER         = $(CURRENT_FOLDER)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "CURRENT_MAKE           = $(CURRENT_MAKE)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "CHARSET_NAME           = $(CHARSET_NAME)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "INCLUDE_USER           = \\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(INCLUDE_USER); do $(ECHO) "                 $$i\\" >> $(CURRENT_MAKE); done
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	
	$(COMMAND_SHOW)$(ECHO) "SUBDIRS                = $(SUBDIRS)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TARGETS                = $(TARGETS)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "TOOLSET                = $(TOOLSET)" >> $(CURRENT_MAKE)
ifneq ($(LIBRARY),)
	$(COMMAND_SHOW)$(ECHO) "LDFLAGS                = -Wl,$(LIBRARY)" >> $(CURRENT_MAKE)
endif

	$(COMMAND_SHOW)$(ECHO) "BINARYS                = \\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(BINARYS); do $(ECHO) "                 $$i\\" >> $(CURRENT_MAKE); done
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	
	$(COMMAND_SHOW)$(ECHO) "OBJECTS                = \\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(OBJECTS); do $(ECHO) "                 $$i\\" >> $(CURRENT_MAKE); done
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	
	$(COMMAND_SHOW)$(ECHO) "HEADERS                = \\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(HEADERS); do $(ECHO) "                 $$i\\" >> $(CURRENT_MAKE); done
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	
	$(COMMAND_SHOW)$(ECHO) "SUFFIXS_HOST		 	 = $(SUFFIXS_HOST)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "SUFFIXS_ARCH		 	 = $(SUFFIXS_ARCH)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "SUFFIXS_MAKE		 	 = $(SUFFIXS_MAKE)" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	
	$(COMMAND_SHOW)$(ECHO) include $(BUILD_ROOT)/$(ARCH)/$(ARCH)_config.mak >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) include $(TOOLSET_MAKE) >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	
	$(COMMAND_SHOW)$(ECHO) ".PHONY: $(SUBDIRS) $(TOOLSET_MAKE) $(PHONYOB) clean" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "all: \\" >> $(CURRENT_MAKE)
ifneq ($(TOOLSET),)
	$(COMMAND_SHOW)$(ECHO) "    $(TOOLSET_MAKE)\\" >> $(CURRENT_MAKE)
endif
ifeq ($(CURRENT_PATH),$(SOURCES_ROOT))
	$(COMMAND_SHOW)$(ECHO) "	$(TOOLSET_MAKE)\\" >> $(CURRENT_MAKE)
endif
	$(COMMAND_SHOW)$(ECHO) "    $(SUBDIRS)\\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(HEADERS) $(OBJECTS); do $(ECHO) "    $$i\\" >> $(CURRENT_MAKE); done
	$(COMMAND_SHOW)$(ECHO) "    $(TARGETS)\\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "    $(TOOLSET)\\" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(ECHO) "" >> $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(DEPENDS); do $(ECHO) "include $$i" >> $(CURRENT_MAKE); done
	$(COMMAND_SHOW)$(ECHO) "include $(SUFFIXS_MAKE)" >> $(CURRENT_MAKE)
ifeq ($(SUFFIXS_MAKE), $(SUFFIXS_HOST))
	$(COMMAND_SHOW)$(MAKE) -f $(CURRENT_MAKE)
endif

remove_config:
	$(COMMAND_SHOW)$(FOR) i in $(TOOLSET) $(OBJECTS) $(TARGETS) $(DEPENDS) $(CONFIGS); \
		do $(ECHO) "Delete file [$$i] ..." && $(RM) -f $$i; \
	done
	$(COMMAND_SHOW)$(FOR) i in $(HEADERS); \
		do $(ECHO) "Delete file [$(CURRENT_PATH)/$$i] ..." && $(RM) -f $$i; \
	done
	$(COMMAND_SHOW)$(ECHO) "Delete file [$(CURRENT_PATH)/$(CURRENT_MAKE)] ..."
	$(COMMAND_SHOW)$(RM) -f $(CURRENT_MAKE)
	$(COMMAND_SHOW)$(FOR) i in $(SUBDIRS); do $(MAKE) -C $$i -f$(PREFIXS_MAKE) remove_config; done
	$(COMMAND_SHOW)$(ECHO) "if [ -d \"$(OBJECTS_PATH)\" ]; then" > ./remove_config.sh
	$(COMMAND_SHOW)$(ECHO) "	$(ECHO) Remove directory [$(OBJECTS_PATH)]" >> ./remove_config.sh
	$(COMMAND_SHOW)$(ECHO) "	$(RD) --ignore-fail-on-non-empty $(OBJECTS_PATH)" >> ./remove_config.sh
ifneq ($(TARGETS_PATH),)
	$(COMMAND_SHOW)$(ECHO) "	$(ECHO) Remove directory [$(TARGETS_PATH)]" >> ./remove_config.sh
	$(COMMAND_SHOW)$(ECHO) "	$(RD) --ignore-fail-on-non-empty $(TARGETS_PATH)" >> ./remove_config.sh
endif
ifneq ($(DEPENDS_PATH),)
	$(COMMAND_SHOW)$(ECHO) "	$(ECHO) Remove directory [$(DEPENDS_PATH)]" >> ./remove_config.sh
	$(COMMAND_SHOW)$(ECHO) "	$(RD) --ignore-fail-on-non-empty $(DEPENDS_PATH)" >> ./remove_config.sh
endif
ifneq ($(TOOLSET_PATH),)
	$(COMMAND_SHOW)$(ECHO) "	$(ECHO) Remove directory [$(TOOLSET_PATH)]" >> ./remove_config.sh
	$(COMMAND_SHOW)$(ECHO) "	$(RD) --ignore-fail-on-non-empty $(TOOLSET_PATH)" >> ./remove_config.sh
endif
	$(COMMAND_SHOW)$(ECHO) "fi" >> ./remove_config.sh
	$(COMMAND_SHOW)./remove_config.sh
	$(COMMAND_SHOW)$(RM) -rf ./remove_config.sh

remove:
	$(COMMAND_SHOW)$(ECHO) "Remove $(ARCH)-$(BOARD) config now ..."
	$(COMMAND_SHOW)$(MAKE) remove_config
	$(COMMAND_SHOW)$(RM) -rf $(OBJECTS_ROOT)
	$(COMMAND_SHOW)$(RM) -f $(TOOLSET_MAKE)

clean:
	$(COMMAND_SHOW)$(MAKE) -f $(CURRENT_MAKE) clean
ifneq ($(wildcard $(TOOLSET_MAKE)),)
	$(COMMAND_SHOW)$(RM) -f $(TOOLSET_MAKE)
endif
	
include $(BUILD_ROOT)/rule/rule_targets.mak
