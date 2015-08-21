MD						=	/c/mgw_x86so/msys/1.0/bin/mkdir.exe
COMMAND_SPEC			=	/c/mgw_x86so/msys/1.0/bin/bash.exe
CURRENT_PATH			=	$(CURDIR)
SOURCES_ROOT			=	$(CURRENT_PATH)/..
SCRIPTS_ROOT			=	$(SOURCES_ROOT)/script
VERSION_PATH			=	$(SOURCES_ROOT)/include
CONFIGS_PATH			=	$(SOURCES_ROOT)/platform/board/$(ARCH)/$(BOARD)
TARGETS					=	$(CONFIGS_PATH)/kboard_name.h						\
							$(CONFIGS_PATH)/kirq_define_enum.h 					\
							$(CONFIGS_PATH)/kirq_define_name.h 					\
							$(CONFIGS_PATH)/$(COMPILER)/kirq_define_enum.inc	\
							$(VERSION_PATH)/version.h
							
all: $(CONFIGS_PATH) $(CONFIGS_PATH)/$(COMPILER) $(TARGETS)

$(CONFIGS_PATH)/%_enum.h: $(CONFIGS_PATH)/%.ini $(SCRIPTS_ROOT)/ini2enum.sh
	$(COMMAND_SPEC) -c "$(SCRIPTS_ROOT)/ini2enum.sh $< $@ :/usr/local/bin:/mingw/bin:/bin:"
	
$(CONFIGS_PATH)/%_name.h: $(CONFIGS_PATH)/%.ini $(SCRIPTS_ROOT)/ini2enum.sh
	$(COMMAND_SPEC) -c "$(SCRIPTS_ROOT)/ini2name.sh $< $@ :/usr/local/bin:/mingw/bin:/bin:"
	
$(CONFIGS_PATH)/$(COMPILER)/%_enum.inc: $(CONFIGS_PATH)/%.ini $(SCRIPTS_ROOT)/ini2inc.$(COMPILER).sh
	$(COMMAND_SPEC) -c "$(SCRIPTS_ROOT)/ini2inc.$(COMPILER).sh $< $@ :/usr/local/bin:/mingw/bin:/bin:"

$(VERSION_PATH)/%.h: $(SOURCES_ROOT)/%.ini $(SCRIPTS_ROOT)/ini2ver.sh
	$(COMMAND_SPEC) -c "$(SCRIPTS_ROOT)/ini2ver.sh $< $@ :/usr/local/bin:/mingw/bin:/bin:"

$(CONFIGS_PATH)/%.h:
	@echo "Create file [$@] ..."
	@echo "#define    BOARD_NAME_STRING    \"$(ARCH)-$(BOARD)\"" > $@

$(CONFIGS_PATH):
	$(MD) $(CONFIGS_PATH)
	
$(CONFIGS_PATH)/$(COMPILER):
	$(MD) $(CONFIGS_PATH)/$(COMPILER)