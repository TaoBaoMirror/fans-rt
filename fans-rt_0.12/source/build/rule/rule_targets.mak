$(TOOLSET_MAKE):
ifneq ($(TOOLSET_NAME),)
	$(COMMAND_SHOW)$(ECHO) "#!/bin/bash" > toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "#This is a automatic script file, created by fans-rt makefile." >> toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "toolname=\`cat $(TOOLSET_MAKE)|grep $(TOOLSET_NAME)|\\" >> toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "   awk -F ' ' '{printf \"%d\", \$$$a1}'\`" >> toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "if [ \"\$$$atoolname\" = \"\" ]; then">> toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "   $(ECHO) Add new configuration '$(TOOLSET_NAME)' to tool_config.mak ..." >> toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "   $(ECHO) $(TOOLSET_NAME) = $(TOOLSET) >> $(TOOLSET_MAKE)" >> toolset--01.sh
	$(COMMAND_SHOW)$(ECHO) "fi" >> toolset--01.sh
	$(COMMAND_SHOW)chmod 0777 toolset--01.sh
	$(COMMAND_SHOW)./toolset--01.sh
	$(COMMAND_SHOW)$(RM) toolset--01.sh
endif
ifeq ($(CURRENT_PATH),$(SOURCES_ROOT))
ifeq ($(wildcard $(TOOLSET_MAKE)),)
	$(COMMAND_SHOW)$(ECHO) "" > $(TOOLSET_MAKE)
endif
endif

$(TOOLSET_ROOT)/%.exe: $(OBJECTS)
	$(COMMAND_SHOW)$(ECHO) "Create target file [$@] ..."
	$(COMMAND_SHOW)$(CC) -o $@ $<

$(OBJECTS_PATH)/%.bin: %.s
	$(COMMAND_SHOW)$(ECHO) "Create object file [$@] ..."
	$(COMMAND_SHOW)$(AS) $(AS_FLAGS) $(AS_INCLUDE) -o $@ $<

$(OBJECTS_PATH)/%.o: %.c 
	$(COMMAND_SHOW)$(ECHO) "Create object file [$@] ..."
	$(COMMAND_SHOW)$(CC) $(CC_FLAGS) $(CC_INCLUDES) -o $@ $< 

%.h: %.$(CHARSET_NAME).in
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CP) $< $@

%.fnt: %.$(CHARSET_NAME).in
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CHARFONT) -charset $(CHARSET_NAME) -show disable -m $< -o $* -p ./ -font $(FONTSET_ROOT)/$(CHARSET_NAME)_16

%.msg: %.$(CHARSET_NAME).in
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CHARFONT) -charset $(CHARSET_NAME) -show disable -m $< -o $* -p ./ -font $(FONTSET_ROOT)/$(CHARSET_NAME)_16