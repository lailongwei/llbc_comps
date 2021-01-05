# File: Makefile
# Brief: easy to build all llbc component libraries and testsuites.
#############################################################################

#****************************************************************************
# Determine platform name
#****************************************************************************
SUPPORTED_PLATFORMS	:= linux darwin
PLATFORMNAME		?= $(shell echo $(shell uname) | tr "[:upper:]" "[:lower:]")
$(if $(findstring $(PLATFORMNAME),$(SUPPORTED_PLATFORMS)),,$(error "Unsupported os, must be one of '$(SUPPORTED_PLATFORMS)'"))

#****************************************************************************
# Default config define
ifndef config
  export config=release64
endif
#****************************************************************************

#****************************************************************************
# some useful variables
#****************************************************************************
# Parse debug option
CONFIG_OPT		?= $(shell echo $(config) | tr "[:upper:]" "[:lower:]")
ifeq ($(CONFIG_OPT),debug)
  DEBUG_OPT=TRUE
else
  DEBUG_OPT=FALSE
endif

# All make targets define
PREMAKE_TARGET := build_makefiles
ALL_COMPS      := $(shell ls comps)
ALL_APPS       := $(subst app_common,,$(shell ls apps))
ALL_TARGETS    := $(PREMAKE_TARGET) $(ALL_COMPS) $(ALL_APPS)
CLEAN_ALL_TARGETS := $(addprefix clean_,$(ALL_COMPS))
CLEAN_ALL_TARGETS += $(addprefix clean_,$(ALL_APPS))

# All targets output directory
ALL_TARGETS_OUTPUT := output/gmake/$(config)
# Some prefixs/suffixes define
ifeq ($(PLATFORMNAME),darwin)
  DYNLIB_SUFFIX := .dylib
else
  DYNLIB_SUFFIX := .so
endif
DEBUG_SUFFIX    := _debug
EXE_SUFFIX      :=

# Some variables define
PREMAKE_PATH	:= "tools/premake"
PREMAKE_NAME	:= "premake5_$(PLATFORMNAME)"

#****************************************************************************
# all real make commands
#****************************************************************************
# Set phonies
.PHONY: help all clean $(PREMAKE_TARGET) $(CLEAN_ALL_TARGETS)
help:
	@echo "make commands:"
	@echo "========================================================================="
	@echo "  make [help]   - display this help information"
	@echo "========================================================================="
	@echo "  make all        - make all components&applications"
	@echo "  make <comp/app> - make specific component/application."
	@echo "                  - now supported comps: $(ALL_COMPS)."
	@echo "                  - now supported applications: $(ALL_APPS)."
	@echo "========================================================================="
	@echo "  make clean            - remove all object directories and target files"
	@echo "  make clean_<comp/application> - remove specific componet/application object directory and target files"

all: $(ALL_TARGETS)

$(PREMAKE_TARGET):
	cd $(PREMAKE_PATH) && ./$(PREMAKE_NAME) gmake

$(ALL_COMPS): $(PREMAKE_TARGET)
	cd build/gmake && make -f $@.make

$(ALL_APPS): $(PREMAKE_TARGET)
	cd build/gmake && make -f $@.make

clean: $(PREAMAKE_TARGET)
	cd 3rds/llbc && make clean
	@(if [ -d build/gmake ]; then cd build/gmake && make clean; fi)
	
$(CLEAN_ALL_TARGETS): $(PREMAKE_TARGET)
	cd build/gmake && make clean -f $(subst clean_,,$@).make
