$(SUBDIRS):
	@$(ECHO) "Build directory [$(CURDIR)/$@] ..."
	@$(ECHO) "@$(MAKE) -C $@ -f $(MAKEFILE)" >> $(COMMAND_LIST)
	@$(MAKE) -C $@ -f $(MAKEFILE)
	
$(OBJECTS_PATH):
	@$(ECHO) "Create directory [$@] ..."
	@$(ECHO) "@$(MD) -p $@" >> $(COMMAND_LIST)
	@$(MD) -p $@
	
$(OBJECTS_PATH)/%.o: %.c $(OBJECTS_PATH)/%.d
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) "@$(CC) $(CC_FLAGS) $(CC_INCLUDES) -o $@ $<" >> $(COMMAND_LIST)
	@$(CC) $(CC_FLAGS) $(CC_INCLUDES) -o $@ $<

$(OBJECTS_PATH)/%.o: %.s
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) "@$(CC) $(AS_FLAGS) $(AS_INCLUDES) -o $@ $<" >> $(COMMAND_LIST)
	@$(CC) $(AS_FLAGS) $(AS_INCLUDES) -o $@ $<

$(TARGETS_PATH)/%.a: $(OBJECTS_PATH)/*.o
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) "@$(AR) -r -s $@ $(OBJECTS_PATH)/*.o" >> $(COMMAND_LIST)
	@$(AR) -r -s $@ $(OBJECTS_PATH)/*.o
	@$(ECHO) "@$(RL) $@" >> $(COMMAND_LIST)
	@$(RL) $@

$(TARGETS_PATH)/%.axf: $(PROJECT_OBJECTS)
	@$(ECHO) "Create file [$@] ..."
	@$(ECHO) "@$(LD) -o $@ $(PROJECT_OBJECTS) $(LD_FLAGS)" >> $(COMMAND_LIST)
	@$(LD) -o $@ $(PROJECT_OBJECTS) $(LD_FLAGS)

remove_command_list:
	@$(RM) -f $(COMMAND_LIST)
	
clean:remove_command_list
	@for i in $(OBJECTS) $(TARGETS); do $(ECHO) "Delete file [$$i] ..." && $(RM) -f $$i; done
	@for i in $(SUBDIRS); do $(ECHO) "Clean directory [$$i] ..." && $(MAKE) -C $$i -f $(MAKEFILE) clean; done