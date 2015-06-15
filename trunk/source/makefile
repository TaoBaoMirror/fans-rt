HELPFILE				=	$(SOURCES_ROOT)/build/build.hlp
MAKEARCH				=	$(wildcard $(SOURCES_ROOT)/build/$(ARCH).mak)
MAKEHELP				=	$(wildcard $(HELPFILE))

ifeq ($(strip $(MAKEARCH)),)
ERROR_REASON			=	error_makearch
endif

ifeq ($(strip $(BOARD)),)
ERROR_VARIABLE			=	"BOARD"
ERROR_REASON			=	error_environment
endif

ifeq ($(strip $(ARCH)),)
ERROR_VARIABLE			=	"ARCH"
ERROR_REASON			=	error_environment
endif

ifeq ($(strip $(SOURCES_ROOT)),)
ERROR_VARIABLE			=	SOURCES_ROOT
ERROR_REASON			=	error_sourceroot
endif

ifeq ($(strip $(ERROR_REASON)),)
include $(MAKEARCH)
else
all: $(ERROR_REASON)
endif


error_sourceroot:
	@echo "ERROR: The environment variable '$(ERROR_VARIABLE)' has not been set."
	@echo "       Try \"export SOURCES_ROOT=$(CURDIR)\" for set this variable."

error_makearch:
	@echo "ERROR: The file \"$(SOURCES_ROOT)/build/$(ARCH).mak\" can not found."
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
