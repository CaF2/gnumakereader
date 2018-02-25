###########################################
#
#	Copyright (c) 2016 Florian Evaldsson
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###########################################

NAME := gnumakereader

VERSION := 0.1

OUTPUT := lib$(NAME).$(VERSION).so

CC := gcc

SRCS := $(wildcard ./src/*.c)

BUILD_DIR := .build
INC_DIR := inc

PKG_CONF_LIBS := glib-2.0 gio-2.0

CFLAGS := -g -Wall
CFLAGS += $(shell pkg-config --cflags $(PKG_CONF_LIBS))
CFLAGS += -I./inc -I./inc/gen
CFLAGS += -fPIC

LDFLAGS := $(shell pkg-config --libs $(PKG_CONF_LIBS))
LDFLAGS += -shared

INCLUDE_TEST := ./test
INCLUDE_TEST_BIN := $(INCLUDE_TEST)/gmrtest

OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.o)))
HEADERS := $(addprefix $(INC_DIR)/gen/,$(notdir $(SRCS:.c=.h)))

## Function for generating header files from a C-file (better functionality than the original makeheaders)
## -Dont override if the file is the same
## @param $1
## 	c/c++ file to read the data from
## @param $2
## 	optional gen.h file, or filecontents to insert
GENERATE_HEADER = $(shell dummy="$$(./string_to_var.sh $1)"; \
	output="$$(echo -e "//This file was generated! Dont touch!\n\#pragma once\n\n\#ifndef $$dummy\n\#define $$dummy$(if $3,\n\n$$(cat $3),)\n\n$$(makeheaders -h $2)" | sed -n '/\#define INTERFACE 0/q;p';echo -e "\n\#endif")";\
	if ! test -f $1 || ! test "$$(cat $1)" = "$$output";\
	then\
		echo "$$output" > $1;\
		echo "echo \"generating $1 ...\"";\
	fi;)

########### TARGETS ETC ###########

all: $(OUTPUT)
	
$(OUTPUT): $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MT $@ -MMD -MF $(BUILD_DIR)/$*.d -c $< -o $@

inc/gen:
	mkdir $@

inc/gen/%.h: src/%.c incgen/%.gen.h | inc/gen 
	@$(call GENERATE_HEADER,$@,$(word 1,$^),$(word 2,$^))
	
inc/gen/%.h: src/%.c | inc/gen
	@$(call GENERATE_HEADER,$@,$(word 1,$^),)

clean:
	rm -f $(OBJS) $(OUTPUT) $(HEADERS)
	
cleanall: clean
	$(MAKE) -C $(INCLUDE_TEST) clean

$(INCLUDE_TEST_BIN): $(OUTPUT) $(HEADERS)
	$(MAKE) -C $(INCLUDE_TEST) 

run: $(INCLUDE_TEST_BIN)
	$(MAKE) -C $(INCLUDE_TEST) $@
	
valgrind: all
	$(MAKE) -C $(INCLUDE_TEST) $@

gdb: all
	$(MAKE) -C $(INCLUDE_TEST) $@

-include $(OBJS:.o=.d)

