ifeq ($(strip $(BOARD)),)
ifneq ($(strip $(board)),)
BOARD			=	$(board)
endif
endif

ifeq ($(strip $(ARCH)),)
ifneq ($(strip $(arch)),)
ARCH			=	$(arch)
endif
endif

ifeq ($(strip $(COMPILER)),)
ifneq ($(strip $(compiler)),)
COMPILER		=	$(compiler)
endif
endif

COMPILER		?=	gcc

SOURCES_ROOT	?=	$(shell pwd)
HELPFILE		=	$(SOURCES_ROOT)/build/build.hlp
MAKEHELP		=	$(wildcard $(HELPFILE))
MAKEFILE		=	$(wildcard $(SOURCES_ROOT)/build/build.mak)
ARCH_SUFFIX		=	$(wildcard $(SOURCES_ROOT)/build/$(ARCH)/$(ARCH)_suffix.mak)
ARCH_CONFIG		=	$(wildcard $(SOURCES_ROOT)/build/$(ARCH)/$(ARCH)_config.mak)


ifeq ($(strip $(ARCH)),)
ERROR_REASON	=	error_environment
ERROR_VARIABLE	=	ARCH
endif

ifeq ($(strip $(BOARD)),)
ERROR_REASON	=	error_environment
ERROR_VARIABLE	=	BOARD
endif

ifeq ($(strip $(MAKEFILE)),)
ERROR_REASON	=	error_makefile
ERROR_VARIABLE	=	build.mak
endif

ifeq ($(strip $(ARCH_CONFIG)),)
ERROR_REASON	=	error_makefile
ERROR_VARIABLE	=	$(ARCH)_config.mak
endif

ifeq ($(strip $(ARCH_SUFFIX)),)
ERROR_REASON	=	error_makefile
ERROR_VARIABLE	=	$(ARCH)_suffix.mak
endif


ifeq ($(strip $(ERROR_REASON)),)
include $(MAKEFILE)
else
all: $(ERROR_REASON)
endif


error_makefile:
	@echo "ERROR: The file \"$(SOURCES_ROOT)/build/$(ERROR_VARIABLE)\" can not found."
	@echo "       Please try \"make help\" for more information."

error_environment:
	@echo "ERROR: The environment variable '$(ERROR_VARIABLE)' has not been set."
	@echo "       Please try \"make help\" for more information."

help:
ifeq ($(strip $(MAKEHELP)),)
ifeq 	($(strip $(ERROR_VARIABLE)),SOURCES_ROOT)
	@make --no-print-directory
else
	@echo "ERROR: The file $(HELPFILE) not found."
endif
else
	@cat $(MAKEHELP)
endif

