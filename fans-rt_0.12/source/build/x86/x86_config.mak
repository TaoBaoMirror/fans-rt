CP					=	cp
RM					=	rm
MV					=	mv
MD					=	mkdir
RD					=	rmdir
CAT					=	cat
ECHO				=	echo
FOR					=	for
COMMAND_SHOW		=
MAKE_FLAGS			=	--no-print-directory
MAKE				+=	$(MAKE_FLAGS)

############################################################################
ifeq ($(SUFFIXS_MAKE), $(SUFFIXS_HOST))
CC_PERFIX			=
CC_ROOT_PATH		=
CC					=	gcc
AS					=	nasm
AR					=	ar
LD					=	ld
OC					=	objcopy
RL					=	ranlib
CC_FLAGS			=	-c
CC_INCLUDES			=	-I$(SOURCES_ROOT)/tools/include
else
############################################################################
CC_PERFIX			=	i386-elf
CC_ROOT_PATH		=	/usr/local/$(CC_PERFIX)-gcc
CC					=	$(CC_ROOT_PATH)/bin/gcc
AS					=	$(CC_ROOT_PATH)/bin/nasm
AR					=	$(CC_ROOT_PATH)/bin/ar
LD					=	$(CC_ROOT_PATH)/bin/ld
OC					=	$(CC_ROOT_PATH)/bin/objcopy
RL					=	$(CC_ROOT_PATH)/bin/ranlib
CC_FLAGS			=	-c
endif
