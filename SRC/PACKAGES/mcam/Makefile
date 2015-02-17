NAME := mcam
VERSION := 0.01
URL := 

TOOLCHAIN := $(shell pwd)/../../../toolchain/Sourcery_G++_Lite-2011.03-41/bin/
export PATH := $(PATH):$(TOOLCHAIN)
export CROSS_COMPILE = arm-none-linux-gnueabi-
export ARCH = arm
export INSTALL_MOD_PATH := $(PWD)/../../ROOTFS

CC	= $(CROSS_COMPILE)gcc
AS	= $(CROSS_COMPILE)as
AR	= $(CROSS_COMPILE)ar
LD	= $(CROSS_COMPILE)ld
STRIP	= $(CROSS_COMPILE)strip
CPP	= $(CROSS_COMPILE)c++
GPP	= $(CROSS_COMPILE)g++

SRCDIR	= ./src
OBJDIR	= ./obj
BINDIR	= ./bin
DIRS	= $(OBJDIR) $(BINDIR)

SOURCES	=$(wildcard $(SRCDIR)/*.c)
INCLUDES=$(wildcard $(SRCDIR)/*.h)
OBJECTS	=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

CFLAGS	= -Os -D_GNU_SOURCE
CFLAGS	+= -I$(TOOLCHAIN)../libc/usr/include
CFLAGS	+= -DDEBUG_MIO

LFLAGS	= -Wall
LFLAGS	+= -L$(TOOLCHAIN)../libc/usr/libs
LFLAGS	+= -lpthread

all: mcam

$(DIRS):
	@(mkdir -p $(DIRS))

$(TOOLCHAIN):
	$(MAKE) -C ../../toolchain

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@($(CC) $(CFLAGS) -c $< -o $@)

.PHONY: mcam
mcam: $(DIRS) $(TOOLCHAIN) $(OBJECTS)
	@($(CC) -o $(BINDIR)/$@ $(LFLAGS) $(OBJECTS))

.PHONY: strip
strip: $(NAME)
	@($(STRIP) $(NAME))

.PHONY: clean
clean:
	@rm -f $(OBJECTS)

.PHONY: distclean
distclean: clean
	@rm -rf $(BINDIR)/$(NAME)
