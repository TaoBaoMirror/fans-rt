SUBDIRS_stm32_mini	=	build system platform libs testcode
SUBDIRS_stm32_lite	=	build platform system libs testcode

TARGETS				=	$(TARGETS_PATH)/fans-rt.$(TARGET_SUFFIX)
SUBDIRS				=	$(SUBDIRS_$(ARCH)_$(BOARD))
CONFIGS				=
OBJECTS_PATH		=
CONFIGS_PATH		=
TARGETS_PATH		=	$(SOURCES_ROOT)/$(ARCH)-$(BOARD)
