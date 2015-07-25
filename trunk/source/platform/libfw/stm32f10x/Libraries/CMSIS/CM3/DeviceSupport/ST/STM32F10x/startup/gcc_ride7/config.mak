#SUBDIRS			=	$(CHILDREN)
OBJECTS_tiny		=	$(OBJECTS_PATH)/startup_stm32f10x_ld.o
OBJECTS_mini		=	$(OBJECTS_PATH)/startup_stm32f10x_md.o
OBJECTS_lite		=	$(OBJECTS_PATH)/startup_stm32f10x_md.o
OBJECTS_huge		=	$(OBJECTS_PATH)/startup_stm32f10x_hd.o
OBJECTS				=	$(OBJECTS_$(BOARD))
DEPENDS				=
TARGETS				=
OBJECTS_PATH		=	$(OBJECTS_ROOT)/boot
CONFIGS_PATH		=
TARGETS_PATH		=


