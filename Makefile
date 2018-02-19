NAME=libhidden-menu-gtk-module.so
OUT=out/$(NAME)
SRCS=src/main.c
INSTALL_PATH=/usr/lib/x86_64-linux-gnu/gtk-3.0/modules/$(NAME)

CFLAGS=-shared -o $(OUT) -fPIC $(SRCS) -g -Wall
GTKFLAGS=`pkg-config --cflags --libs gtk+-x11-3.0`
CC=gcc

all: main

main: $(SRCS)
	$(CC) $(CFLAGS) $(GTKFLAGS)

install: $(OUT)
	cp -v $(OUT) $(INSTALL_PATH)

clean:
	rm -fv $(OUT)

distclean:
	rm -v $(INSTALL_PATH)
