SUBDIRS_stm32_mini	=	build system libs testcode platform 
TARGETS_stm32_mini	=	$(ARCH)-$(BOARD)$(EXECUTE_TARGET)
SUBDIRS_stm32_lite	=	build system libs testcode platform 
TARGETS_stm32_lite	=	$(ARCH)-$(BOARD)$(EXECUTE_TARGET)

SUBDIRS				=	$(SUBDIRS_$(ARCH)_$(BOARD))
CONFIGS				=
TARGETS				=	$(TARGETS_PATH)/$(TARGETS_$(ARCH)_$(BOARD))
OBJECTS_PATH		=
CONFIGS_PATH		=
TARGETS_PATH		=	$(SOURCES_ROOT)/../objects/$(ARCH)-$(BOARD)
