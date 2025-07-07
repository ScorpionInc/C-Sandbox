location = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
$(warning $(location))

SHELL ?= bash -x
CC ?= gcc

# Defines default target as 'all'
default_target: all

# Gives some help text.
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... help (displays this message)"

# Variables used to GLOB source files.
INPUTS := $(wildcard ./src/*.c)
#INPUT_NAMES := $(notdir $(INPUTS))
#OUTPUT_NAMES := $(patsubst %.c, %.o, $(INPUT_NAMES))
OUTPUTS := $(INPUTS:./src%.c=build%.o)
#$(warning "INPUTS $(INPUTS)")
#$(warning "INPUT_NAMES $(INPUT_NAMES)")
#$(warning "OUTPUT_NAMES $(OUTPUT_NAMES)")
#$(warning "OUTPUTS $(OUTPUTS)")

# Make all c files in src into object files in build
build/%.o: src/%.c
	$(CC) -c -Iinclude -o./$@ ./$<

# Require all c files to be object files then link to a static lib
build/libsi_cbig.a: $(OUTPUTS)
	@echo "Make is building target 'libsi_cbig.a'."
	@echo "Outputs: $(OUTPUTS)"
	ar rcs ./build/libsi_cbig.a $(OUTPUTS)

# Defaults to static lib
all: build/libsi_cbig.a
	@echo "Make is building target 'all'."
