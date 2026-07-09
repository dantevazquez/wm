# wm - simple window manager Makefile

# Paths
PREFIX ?= $(HOME)/.local
BINDIR ?= $(PREFIX)/bin

# Tools
CC ?= gcc
PKG_CONFIG ?= pkg-config

# Find dependencies using pkg-config
PACKAGES = x11

# Compiler/Linker Flags
CFLAGS += -std=c99 -Wall -Wextra -O2
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(PACKAGES) 2>/dev/null || echo -I/usr/X11R6/include)
LDFLAGS += $(shell $(PKG_CONFIG) --libs $(PACKAGES) 2>/dev/null || echo -L/usr/X11R6/lib -lX11 -lXft -lXinerama -lXext) -lpthread

# Target and Sources
TARGET = monowm
SRCS = main.c appicons.c bar.c keys.c
OBJS = $(SRCS:.c=.o)
HEADERS = appicons.h config.h bar.h keys.h

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	install -m 755 monowm-start $(DESTDIR)$(BINDIR)/monowm-start
	install -m 755 monowm-volume $(DESTDIR)$(BINDIR)/monowm-volume
	install -d $(HOME)/.config/sxhkd
	install -m 644 sxhkdrc $(HOME)/.config/sxhkd/sxhkdrc
	echo '#!/bin/sh' > $(HOME)/.xinitrc
	echo 'exec $(BINDIR)/monowm-start' >> $(HOME)/.xinitrc
	chmod +x $(HOME)/.xinitrc

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -f $(DESTDIR)$(BINDIR)/monowm-start
	rm -f $(DESTDIR)$(BINDIR)/monowm-volume

.PHONY: all clean install uninstall
