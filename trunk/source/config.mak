SUBDIRS_stm32		=	build boot system platform libs testcode

TARGETS				=	$(TARGETS_PATH)/fans-rt.$(TARGET_SUFFIX)
SUBDIRS				=	$(SUBDIRS_$(ARCH))
CONFIGS				=
OBJECTS_PATH		=
CONFIGS_PATH		=
TARGETS_PATH		=	$(SOURCES_ROOT)/$(ARCH)-$(BOARD)
