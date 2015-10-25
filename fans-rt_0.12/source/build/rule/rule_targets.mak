$(TOOLSET_MAKE):
ifneq ($(TOOLSET_NAME),)
	$(COMMAND_SHOW)$(ECHO) "#!/bin/bash" > toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "#This is a automatic script file, created by fans-rt makefile." >> toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "toolname=\`cat $(TOOLSET_MAKE)|grep $(TOOLSET_NAME)|\\" >> toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "   awk -F ' ' '{printf \"%d\", \$$$a1}'\`" >> toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "if [ \"\$$$atoolname\" = \"\" ]; then">> toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "   $(ECHO) Add new configuration '$(TOOLSET_NAME)' to tool_config.mak ..." >> toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "   $(ECHO) $(TOOLSET_NAME) = $(TOOLSET) >> $(TOOLSET_MAKE)" >> toolsetconfig.sh
	$(COMMAND_SHOW)$(ECHO) "fi" >> toolsetconfig.sh
	$(COMMAND_SHOW)chmod 0777 toolsetconfig.sh
	$(COMMAND_SHOW)./toolsetconfig.sh
	$(COMMAND_SHOW)$(RM) toolsetconfig.sh
endif
ifeq ($(CURRENT_PATH),$(SOURCES_ROOT))
ifeq ($(wildcard $(TOOLSET_MAKE)),)
	$(COMMAND_SHOW)$(ECHO) "" > $(TOOLSET_MAKE)
endif
endif

$(TOOLSET_ROOT)/$(CURRENT_FOLDER).a: $(OBJECTS)
	$(COMMAND_SHOW)$(ECHO) "Create file [$@] ..."
	$(COMMAND_SHOW)$(ECHO) The object files is :
	$(COMMAND_SHOW)$(ECHO) $(OBJECTS)
	$(COMMAND_SHOW)$(ECHO) "@$(AR) -r -s $@ $(OBJECTS_PATH)/*.o" >> $(COMMAND_LIST)
	$(COMMAND_SHOW)$(AR) -r -s $@ $(OBJECTS_PATH)/*.o
	$(COMMAND_SHOW)$(ECHO) "@$(RL) $@" >> $(COMMAND_LIST)
	$(COMMAND_SHOW)$(RL) $@

$(TARGETS_PATH)/%.ex_: $(OBJECTS)
	$(COMMAND_SHOW)$(ECHO) "Create tools file [$@] ..."
	$(COMMAND_SHOW)$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(TOOLSET_PATH)/%.exe: $(OBJECTS)
	$(COMMAND_SHOW)$(ECHO) "Create tools file [$@] ..."
	$(COMMAND_SHOW)$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(TOOLSET_ROOT)/%.exe: $(TARGETS_PATH)/%.ex_ $(BINARYS)
	$(COMMAND_SHOW)$(ECHO) "Packet tools file [$@] ..."
	$(COMMAND_SHOW)$(CP) $< $@
	$(COMMAND_SHOW)$(FOR) i in $(BINARYS); do cat $$i >> $@; done

%.s: $(CONFIGS_PATH)/%.s.in
	$(COMMAND_SHOW)$(ECHO) "Create source file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CP) $< $@
	
%.inc: $(CONFIGS_PATH)/%.inc.in
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CP) $< $@

%.txt: $(CONFIGS_PATH)/%.txt.in
	$(COMMAND_SHOW)$(ECHO) "Create text file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CP) $< $@

$(OBJECTS_PATH)/%.bin: %.s
	$(COMMAND_SHOW)$(ECHO) "Create object file [$@] ..."
	$(COMMAND_SHOW)$(AS) $(AS_FLAGS) $(AS_INCLUDE) $(INCLUDE_USER) -o $@ $<
	
$(OBJECTS_PATH)/%.o: %.s
	$(COMMAND_SHOW)$(ECHO) "Create object file [$@] ..."
	$(COMMAND_SHOW)$(AS) $(AS_FLAGS) $(AS_INCLUDE) $(INCLUDE_USER) -f coff -o $@ $<

$(OBJECTS_PATH)/%.o: %.c 
	$(COMMAND_SHOW)$(ECHO) "Create object file [$@] ..."
	$(COMMAND_SHOW)$(CC) $(CC_FLAGS) $(CC_INCLUDES) $(INCLUDE_USER) -o $@ $< 

%.h: %.$(CHARSET_NAME).in
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CP) $< $@

%.fnt: %.$(CHARSET_NAME).txt
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CHARFONT) -charset $(CHARSET_NAME) -show disable -m $< -o $* -p ./ -font $(FONTSET_ROOT)/$(CHARSET_NAME)_16

%.msg: %.$(CHARSET_NAME).txt
	$(COMMAND_SHOW)$(ECHO) "Create header file [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(CHARFONT) -charset $(CHARSET_NAME) -show disable -m $< -o $* -p ./ -font $(FONTSET_ROOT)/$(CHARSET_NAME)_16

	