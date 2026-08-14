# xmenu - terminal menu utility

include config.mk

SRC = libxmenu.c xmenu_cli.c
OBJ = $(SRC:.c=.o)

all: xmenu

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): xmenu.h config.mk
xmenu_cli.o: termbox2.h

xmenu: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f xmenu $(OBJ) xmenu-$(VERSION).tar.gz

dist: clean
	mkdir -p xmenu-$(VERSION)
	cp -R LICENSE Makefile config.mk xmenu.h termbox2.h $(SRC) xmenu-$(VERSION)
	tar -cf xmenu-$(VERSION).tar xmenu-$(VERSION)
	gzip xmenu-$(VERSION).tar
	rm -rf xmenu-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f xmenu $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xmenu

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xmenu

.PHONY: all clean dist install uninstall
