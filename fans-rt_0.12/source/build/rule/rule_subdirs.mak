$(SUBDIRS):
	$(COMMAND_SHOW)$(ECHO) "Build directory [$(CURRENT_PATH)/$@] ..."
	$(COMMAND_SHOW)$(ECHO) "$(MAKE) -C $@ -f $(CURRENT_MAKE)" >> $(COMMAND_LIST)
	$(COMMAND_SHOW)$(MAKE) -C $@ -f $(CURRENT_MAKE)

