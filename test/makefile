###########################################
#
#	Copyright (c) 2016 Florian Evaldsson
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###########################################

CC = gcc
MEMTEST = valgrind

NAME = gmr

INSTALL_DIR ?= /usr/bin

FOLDER = .

PARAMS = -p.. -v

######

SRCS = $(wildcard $(FOLDER)/*.c)

OBJS = $(SRCS:.c=.o)

CFLAGS = -g -std=c11
CFLAGS += -I./../inc -I./../inc/gen
CFLAGS += $(shell pkg-config --cflags glib-2.0)

LDFLAGS = ../libgnumakereader.0.1.so
LDFLAGS += $(shell pkg-config --libs glib-2.0)

######

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

run: all
	./$(NAME) $(PARAMS)
	
clean:
	rm -f $(OBJS) $(NAME)

valgrind: all
	$(MEMTEST) --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all --suppressions=/home/caf2/dev/helpers/gtk.suppression ./$(NAME) $(PARAMS)
	
gdb: all
	gdb ./$(NAME) $(PARAMS)

$(INSTALL_DIR)/$(NAME):
	cp $(NAME) $@
	
install: $(INSTALL_DIR)/$(NAME)

