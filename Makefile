include config.mk

SRC = xmenu.c
OBJ = $(SRC:.c=.o)

all: xmenu

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): config.mk

xmenu: xmenu.o
	$(CC) -o $@ xmenu.o $(LDFLAGS)

clean:
	rm -f xmenu $(OBJ) xmenu-$(VERSION).tar.gz

dist: clean
	mkdir -p xmenu-$(VERSION)
	cp LICENSE Makefile config.mk $(SRC) termbox2.h xmenu-$(VERSION)
	tar -cf xmenu-$(VERSION).tar xmenu-$(VERSION)
	gzip xmenu-$(VERSION).tar
	rm -rf xmenu-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f xmenu $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xmenu

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xmenu

# temporary: libxmenu test target
SRC_LIB = libxmenu.c
SRC_CLI = xmenu_cli.c
OBJ_LIB = $(SRC_LIB:.c=.o)
OBJ_CLI = $(SRC_CLI:.c=.o)

libxmenu: xmenu_cli

xmenu_cli: $(OBJ_CLI) libxmenu.a
	$(CC) -o $@ $(OBJ_CLI) -L. -lxmenu $(LDFLAGS)

libxmenu.a: $(OBJ_LIB)
	$(AR) rcs $@ $^

$(OBJ_LIB): xmenu.h config.mk
$(OBJ_CLI): xmenu.h config.mk

.PHONY: all clean dist install uninstall libxmenu
