SUBDIRS				=
TARGETS				=
CONFIGS				=	$(CONFIGS_PATH)/kboard_name.h								\
						$(CONFIGS_PATH)/kirq_define_enum.h 							\
						$(CONFIGS_PATH)/kirq_define_name.h 							\
						$(CONFIGS_PATH)/$(COMPILER)/kirq_define_enum.inc 			\
						
CONFIGS_INFO		=	$(CONFIGS_PATH)/%.ini
CONFIGS_PATH		=	$(SOURCES_ROOT)/platform/board/$(ARCH)/$(BOARD)
OBJECTS_PATH		=