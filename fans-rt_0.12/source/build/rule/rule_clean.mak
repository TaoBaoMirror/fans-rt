
clean:
	$(COMMAND_SHOW)for i in $(TOOLSET) $(OBJECTS) $(TARGETS); do $(ECHO) "Delete file [$$i] ..." && $(RM) -f $$i; done
	$(COMMAND_SHOW)for i in $(HEADERS); do $(ECHO) "Delete file [$(CURRENT_PATH)/$$i] ..." && $(RM) -f $$i; done
	$(COMMAND_SHOW)$(FOR) i in $(SUBDIRS); do $(ECHO) "Clean directory [$(CURRENT_PATH)/$$i] ..." && $(MAKE) -C $$i -f $(CURRENT_MAKE) clean; done
