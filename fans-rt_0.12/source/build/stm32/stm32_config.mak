HOSTCC_PERFIX			=
HOSTCC_ROOT_PATH		=
HOSTCC					?=	gcc
HOSTAS					?=	nasm
HOSTAR					?=	ar
HOSTLD					?=	ld
HOSTOC					?=	objcopy
HOSTRL					?=	ranlib
CP						?=	cp
RM						?=	rm
MV						?=	mv
MD						?=	mkdir
RD						?=	rmdir
CAT						?=	cat
ECHO					?=	echo


ARCHCC_PERFIX			?=	i386-elf
ARCHCC_ROOT_PATH		?=	/usr/local/$(ARCHCC_PERFIX)-gcc
ARCHCC					?=	$(ARCHCC_ROOT_PATH)/bin/$(ARCHCC_PERFIX)-gcc
ARCHAS					?=	$(ARCHCC_ROOT_PATH)/bin/$(ARCHCC_PERFIX)-nasm
ARCHAR					?=	$(ARCHCC_ROOT_PATH)/bin/$(ARCHCC_PERFIX)-ar
ARCHLD					?=	$(ARCHCC_ROOT_PATH)/bin/$(ARCHCC_PERFIX)-ld
ARCHOC					?=	$(ARCHCC_ROOT_PATH)/bin/$(ARCHCC_PERFIX)-objcopy
ARCHRL					?=	$(ARCHCC_ROOT_PATH)/bin/$(ARCHCC_PERFIX)-ranlib