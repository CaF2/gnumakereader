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

CFLAGS := -g -Wall
CFLAGS += $(shell pkg-config --cflags glib-2.0)
CFLAGS += -I./inc -I./inc/gen
CFLAGS += -fPIC

LDFLAGS := $(shell pkg-config --libs glib-2.0)
LDFLAGS += -shared

INCLUDE_TEST := ./test
INCLUDE_TEST_BIN := $(INCLUDE_TEST)/gmrtest

OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.o)))
HEADERS := $(addprefix $(INC_DIR)/gen/,$(notdir $(SRCS:.c=.h)))

GENERATE_HEADER = dummy="$$(./string_to_var.sh $1)"; echo -e \
		"//This file was generated! Dont touch!\n\#pragma once\n\n\#ifndef $$dummy\n\#define $$dummy$(if $3,\n\n$$(cat $3),)\n\n$$(makeheaders -h $2)" | sed -n '/\#define INTERFACE 0/q;p' > $1;echo -e "\n\#endif" >> $1; echo "generating $1 ..."

all: $(OUTPUT)
	
$(OUTPUT): $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

inc/gen/%.h: src/%.c incgen/%.gen.h
	@$(call GENERATE_HEADER,$@,$(word 1,$^),$(word 2,$^))
	
inc/gen/%.h: src/%.c
	@$(call GENERATE_HEADER,$@,$(word 1,$^),)
		
test:
	echo $(wildcard incgen/shell.gen.h) $(wildcard src/shell.c)

clean:
	rm -f $(OBJS) $(OUTPUT) $(HEADERS)
	
cleanall: clean
	$(MAKE) -C $(INCLUDE_TEST) clean

$(INCLUDE_TEST_BIN): $(OUTPUT) $(HEADERS)
	$(MAKE) -C $(INCLUDE_TEST) 

run: $(INCLUDE_TEST_BIN)
	$(MAKE) -C $(INCLUDE_TEST) run
	
valgrind: all
	valgrind  --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUTPUT)